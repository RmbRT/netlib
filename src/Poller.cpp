#include "Poller.hpp"
#include "Socket.hpp"

#ifdef NETLIB_EPOLL
#include <sys/epoll.h>
#include "internal/platform.hpp"

#define INVALID_POLLER -1
#else
#include <poll.h>
#endif
#include <cassert>

namespace netlib
{
	Poller::Poller():
#ifdef NETLIB_EPOLL
		m_poller(INVALID_POLLER),
		m_event_list(nullptr),
		m_event_list_capacity(0)
#else
		m_poll_list(nullptr),
		m_poll_list_capacity(0)
#endif
	{
	}

	Poller::Poller(
		Socket * const * sockets,
		std::size_t count):
		Poller()
	{
		watch(sockets, count);
	}

	Poller::Poller(
		Poller && move):
		m_sockets(std::move(move.m_sockets)),
#ifdef NETLIB_EPOLL
		m_poller(move.m_poller),
		m_event_list(move.m_event_list),
		m_event_list_capacity(move.m_event_list_capacity)
#else
		m_poll_list(move.m_poll_list),
		m_poll_list_capacity(move.m_poll_list_capacity)
#endif

	{
		// Reinitialise the other poller to empty.
		new (&move) Poller();
	}

	Poller &Poller::operator=(
		Poller && move)
	{
		if(this == &move)
			return *this;

		unwatch_all();
		m_sockets = std::move(move.m_sockets);

#ifdef NETLIB_EPOLL
		m_poller = move.m_poller;
		m_event_list = move.m_event_list;
		m_event_list_capacity = move.m_event_list_capacity;
#else
		m_poll_list = move.m_poll_list;
		m_poll_list_capacity = move.m_poll_list_capacity;
#endif
		new (&move) Poller();

		return *this;
	}

	Poller::~Poller()
	{
		unwatch_all();
	}

	bool Poller::watching(
		Socket * socket) const
	{
		assert(socket != nullptr);
		assert(socket->exists());

		return m_sockets.find(socket->m_socket) != m_sockets.end();
	}

	bool Poller::watch(
		Socket * const * sockets,
		std::size_t count)
	{
		assert(sockets != nullptr);

		reserve(m_sockets.size() + count);

		for(std::size_t i = 0; i < count; i++)
			if(!watch(sockets[i]))
				return false;

		return true;
	}

	bool Poller::watch(
		Socket * socket)
	{
		assert(socket != nullptr);
		assert(socket->exists());

		reserve(m_sockets.size() + 1);

#ifdef NETLIB_EPOLL
		if(m_poller == INVALID_POLLER)
		{
			m_poller = ::epoll_create1(0);
			if(m_poller == INVALID_POLLER)
				return false;
		}

		::epoll_event event;
		event.events = EPOLLIN;
		event.data.fd = socket->m_socket;

		if(INVALID_POLLER == epoll_ctl(m_poller, EPOLL_CTL_ADD, socket->m_socket, &event))
			return false;

		m_sockets.insert(
			std::make_pair(socket->m_socket, socket));
#else
		static_cast<::pollfd *>(m_poll_list)[m_sockets.size()].fd = socket->m_socket;
		static_cast<::pollfd *>(m_poll_list)[m_sockets.size()].events = POLLIN;

		m_sockets.insert(
			std::make_pair(socket->m_socket, socket));
#endif
		return true;
	}

	bool Poller::unwatch(
		Socket * const * sockets,
		std::size_t count)
	{
		assert(sockets != nullptr);

		for(std::size_t i = 0; i < count; i++)
			if(!unwatch(sockets[i]))
				return false;

		return true;
	}

	bool Poller::unwatch(
		Socket * socket)
	{
		assert(socket != nullptr);
		assert(socket->exists());

#ifdef NETLIB_EPOLL
		::epoll_event event;
		event.events = EPOLLIN;
		event.data.fd = socket->m_socket;

		// Return false if the socket was not watched.
		if(-1 == epoll_ctl(m_poller, EPOLL_CTL_DEL, socket->m_socket, &event))
			return false;
#else
		std::size_t index;
		for(index = 0; index < m_sockets.size(); index++)
			if(static_cast<::pollfd *>(m_poll_list)[index].fd == socket->m_socket)
			{
				std::size_t end = m_sockets.size()-1;
				for(std::size_t i = index; i < end; i++)
					static_cast<::pollfd *>(m_poll_list)[i] = static_cast<::pollfd *>(m_poll_list)[i+1];
				break;
			}

		// Return false if the socket was not watched.
		if(index == m_sockets.size())
			return false;
#endif
		m_sockets.erase(m_sockets.find(socket->m_socket));

		return true;
	}

	void Poller::unwatch_all()
	{
		m_sockets.clear();
#ifdef NETLIB_EPOLL
		if(m_poller != INVALID_POLLER)
		{
			m_event_list_capacity = 0;
			::close(m_poller);
			m_poller = INVALID_POLLER;
			std::free(m_event_list);
			m_event_list = nullptr;
		}
#else
		if(m_poll_list)
		{
			m_poll_list_capacity = 0;
			std::free(m_poll_list);
			m_poll_list = nullptr;
		}
#endif
	}

	bool Poller::poll(
		std::vector<Socket *> &pending,
		std::size_t ms_timeout)
	{
		if(m_sockets.empty())
			return true;

#ifdef NETLIB_EPOLL
		assert(m_poller != INVALID_POLLER);

		std::size_t count = ::epoll_wait(
			m_poller,
			(::epoll_event *) m_event_list,
			m_sockets.size(),
			ms_timeout);

		if(count == -1)
			return false;

		pending.resize(count);

		for(std::size_t i = 0; i < count; i++)
			pending[i] = m_sockets[static_cast<::epoll_event *>(m_event_list)[i].data.fd];
#else
		std::size_t count = ::poll(
			(::pollfd *) m_poll_list,
			m_sockets.size(),
			ms_timeout);

		if(count == -1)
			return false;

		pending.resize(0);
		pending.reserve(count);

		for(std::size_t i = 0; count; i++)
			if(static_cast<::pollfd *>(m_poll_list)[i].revents & POLLIN)
			{
				--count;
				pending.push_back(m_sockets[static_cast<::pollfd *>(m_poll_list)[i].fd]);
			}
#endif

		return true;
	}

	void Poller::reserve(
		std::size_t size)
	{
		m_sockets.reserve(size);
#ifdef NETLIB_EPOLL
		if(size <= m_event_list_capacity)
			return;

		m_event_list = std::realloc(
			m_event_list,
			sizeof(::epoll_event) * size);

		if(!m_event_list && size)
			throw std::bad_alloc();

		m_event_list_capacity = size;
#else
		if(size <= m_poll_list_capacity)
			return;

		m_poll_list = std::realloc(
			m_poll_list,
			sizeof(::pollfd) * size);

		if(!m_poll_list && size)
			throw std::bad_alloc();

		m_poll_list_capacity = size;
#endif
	}
}