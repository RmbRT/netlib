#include "Poller.hpp"

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
		m_poller(INVALID_POLLER)
#else
		m_poll_list(nullptr),
		m_poll_list_size(0),
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

	Poller::~Poller()
	{
		unwatch_all();
	}

	bool Poller::watch(
		Socket * const * sockets,
		std::size_t count)
	{
		assert(sockets != nullptr);

#ifndef NETLIB_EPOLL
		reserve(m_poll_list_size + count);
#endif
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
		reserve(m_poll_list_size + 1);

		static_cast<::pollfd *>(m_poll_list)[m_poll_list_size].fd = socket->m_socket;
		static_cast<::pollfd *>(m_poll_list)[m_poll_list_size].events = POLLIN;

		m_sockets.insert(
			std::make_pair(socket->m_socket, socket));

		++m_poll_list_size;
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
		assert(m_poller != INVALID_POLLER);

		::epoll_event event;
		event.events = EPOLLIN;
		event.data.fd = socket->m_socket;

		if(-1 == epoll_ctl(m_poller, EPOLL_CTL_DEL, socket->m_socket, &event))
			return false;

		m_sockets.erase(m_sockets.find(socket->m_socket));
#else
		assert(m_poll_list_size != 0);

		std::size_t index;
		for(index = 0; index < m_poll_list_size; index++)
			if(static_cast<::pollfd *>(m_poll_list)[index].fd == socket->m_socket)
			{
				--m_poll_list_size;
				for(std::size_t i = index; i < m_poll_list_size; i++)
					static_cast<::pollfd *>(m_poll_list)[i] = static_cast<::pollfd *>(m_poll_list)[i+1];
			}
		m_sockets.erase(m_sockets.find(socket->m_socket));
#endif
		return true;
	}

	bool Poller::unwatch_all()
	{
		m_sockets.clear();
#ifdef NETLIB_EPOLL
		if(m_poller != INVALID_POLLER)
		{
			::close(m_poller);
			m_poller = INVALID_POLLER;
		}
#else
		m_poll_list_size = m_poll_list_capacity = 0;
		std::free(m_poll_list);
		m_poll_list = nullptr;
#endif
	}
}