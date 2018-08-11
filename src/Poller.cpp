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
		m_event_list_size(0),
		m_event_list_capacity(0)
#else
		m_poll_list(nullptr),
		m_poll_list_capacity(0)
#endif
	{
	}

	Poller::Poller(
		Socket * const * sockets,
		std::size_t count,
		bool read,
		bool write):
		Poller()
	{
		watch(sockets, count, read, write);
	}

	Poller::Poller(
		Poller && move):
#ifdef NETLIB_EPOLL
		m_poller(move.m_poller),
		m_event_list(move.m_event_list),
		m_event_list_size(move.m_event_list_size),
		m_event_list_capacity(move.m_event_list_capacity)
#else
		m_sockets(std::move(move.m_sockets)),
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

#ifdef NETLIB_EPOLL
		m_poller = move.m_poller;
		m_event_list = move.m_event_list;
		m_event_list_size = move.m_event_list_size;
		m_event_list_capacity = move.m_event_list_capacity;
#else
		m_sockets = std::move(move.m_sockets);
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

	bool Poller::watch(
		Socket * const * sockets,
		std::size_t count,
		bool read,
		bool write)
	{
		assert(sockets != nullptr);

#ifdef NETLIB_EPOLL
		reserve(m_event_list_size + count);
#else
		reserve(m_sockets.size() + count);
#endif

		for(std::size_t i = 0; i < count; i++)
			if(!watch(sockets[i], read, write))
				return false;

		return true;
	}

	bool Poller::watch(
		Socket * socket,
		bool read,
		bool write)
	{
		assert(socket != nullptr);
		assert(socket->exists());

#ifdef NETLIB_EPOLL
		reserve(m_event_list_size + 1);

		if(m_poller == INVALID_POLLER)
		{
			m_poller = ::epoll_create1(0);
			if(m_poller == INVALID_POLLER)
				return false;
		}

		::epoll_event event;

		event.events = 0;
		if(read)
			event.events |= EPOLLIN;
		if(write)
			event.events |= EPOLLOUT;

		event.data.ptr = socket;

		if(INVALID_POLLER == epoll_ctl(m_poller, EPOLL_CTL_ADD, socket->m_socket, &event))
			return false;

		++m_event_list_size;
#else
		reserve(m_sockets.size() + 1);

		::pollfd & it = static_cast<::pollfd *>(m_poll_list)[m_sockets.size()];

		it.fd = socket->m_socket;

		it.events = 0;
		if(read)
			it.events |= POLLIN;
		if(write)
			it.events |= POLLOUT;

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
		static ::epoll_event event;

		// Return false if the socket was not watched.
		if(-1 == epoll_ctl(m_poller, EPOLL_CTL_DEL, socket->m_socket, &event))
			return false;

		--m_event_list_size;
#else
		bool found = false;
		std::size_t index;
		for(index = 0; index < m_sockets.size(); index++)
			if(static_cast<::pollfd *>(m_poll_list)[index].fd == socket->m_socket)
			{
				std::size_t end = m_sockets.size()-1;
				for(std::size_t i = index; i < end; i++)
					static_cast<::pollfd *>(m_poll_list)[i] = static_cast<::pollfd *>(m_poll_list)[i+1];
				found = true;
				break;
			}

		// Return false if the socket was not watched.
		if(!found)
			return false;

		m_sockets.erase(m_sockets.find(socket->m_socket));
#endif

		return true;
	}

	void Poller::unwatch_all()
	{
#ifdef NETLIB_EPOLL
		if(m_poller != INVALID_POLLER)
		{
			m_event_list_size = 0;
			m_event_list_capacity = 0;
			::close(m_poller);
			m_poller = INVALID_POLLER;
			std::free(m_event_list);
			m_event_list = nullptr;
		}
#else
		m_sockets.clear();
		if(m_poll_list)
		{
			m_poll_list_capacity = 0;
			std::free(m_poll_list);
			m_poll_list = nullptr;
		}
#endif
	}

	bool Poller::poll(
		std::vector<PollEvent> &events,
		std::size_t ms_timeout)
	{
#ifdef NETLIB_EPOLL
		if(!m_event_list_size)
			return true;

		assert(m_poller != INVALID_POLLER);

		std::size_t count = ::epoll_wait(
			m_poller,
			static_cast<::epoll_event *>(m_event_list),
			m_event_list_size,
			ms_timeout);

		if(count == -1)
			return false;

		events.reserve(count);

		for(std::size_t i = 0; i < count; i++)
		{
			::epoll_event & it = static_cast<::epoll_event *>(m_event_list)[i];

			if(it.events & (EPOLLIN | EPOLLOUT | EPOLLERR))
			{
				PollEvent event;
				event.socket = static_cast<Socket *>(it.data.ptr);
				event.can_read = it.events & EPOLLIN;
				event.can_write = it.events & EPOLLOUT;
				event.error = it.events & EPOLLERR;

				events.push_back(event);
			}
		}
#else
		if(m_sockets.empty())
			return true;

		std::size_t count = ::poll(
			(::pollfd *) m_poll_list,
			m_sockets.size(),
			ms_timeout);

		if(count == -1)
			return false;

		pending.resize(0);
		pending.reserve(count);

		for(std::size_t i = 0; count; i++)
		{
			::pollfd & it = static_cast<::pollfd *>(m_poll_list)[i];
			if(it.revents & (POLLIN | POLLOUT | POLLERR))
			{
				--count;

				PollEvent event;
				event.socket = m_sockets[it.fd];
				event.can_read = it.events & POLLIN;
				event.can_write = it.events & POLLOUT;
				event.error = it.events & POLLERR;

				events.push_back(event);
			}
		}
#endif

		return true;
	}

	void Poller::reserve(
		std::size_t size)
	{
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

		m_sockets.reserve(size);

		m_poll_list = std::realloc(
			m_poll_list,
			sizeof(::pollfd) * size);

		if(!m_poll_list && size)
			throw std::bad_alloc();

		m_poll_list_capacity = size;
#endif
	}
}