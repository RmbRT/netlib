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
	namespace detail
	{
		WatchEntry::WatchEntry(
			socket_t socket_handle,
			Socket * socket):
			socket_handle(socket_handle),
			socket(socket),
			iterator()
		{
		}
	}

	bool PollEvent::operator()() const
	{
		if(can_read)
			entry->socket->m_input.notify_one();
		if(can_write)
			entry->socket->m_output.notify_one();

		if(error)
		{
			entry->socket->m_input.fail_one();
			entry->socket->m_output.fail_one();
		}
	}

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

	detail::WatchEntry const * Poller::watch(
		Socket * socket,
		bool read,
		bool write)
	{
		assert(socket != nullptr);
		assert(socket->exists());

		// Add the socket to the watch list.
		m_watch_list.emplace_front(socket->m_socket, socket);
		m_watch_list.front().iterator = m_watch_list.begin();

		// Make space for 1 more entry in the poll/event list.
		reserve_additional(1);

#ifdef NETLIB_EPOLL

		// Make sure the poller object exists.
		if(m_poller == INVALID_POLLER)
		{
			m_poller = ::epoll_create1(0);
			if(m_poller == INVALID_POLLER)
			{
				m_watch_list.erase(m_watch_list.begin());
				return nullptr;
			}
		}

		// Create the event listener.
		::epoll_event event;

		event.events = 0;
		if(read)
			event.events |= EPOLLIN;
		if(write)
			event.events |= EPOLLOUT;

		// Remember the watch list entry that belongs to the socket.
		event.data.ptr = &m_watch_list.front();

		// Add the socket to the poller.
		if(INVALID_POLLER == epoll_ctl(m_poller, EPOLL_CTL_ADD, socket->m_socket, &event))
		{
			m_watch_list.erase(m_watch_list.begin());
			return nullptr;
		}

		++m_event_list_size;
#else

		// Add the socket to the end of the poll list.
		::pollfd & it = static_cast<::pollfd *>(m_poll_list)[m_sockets.size()];

		it.fd = socket->m_socket;

		// Configure the events.
		it.events = 0;
		if(read)
			it.events |= POLLIN;
		if(write)
			it.events |= POLLOUT;

		// Remember the watch list entry that belongs to the socket.
		m_sockets.insert(
			std::make_pair(socket->m_socket, m_watch_list.begin()));
#endif

		// Return the socket's watch list entry.
		return &m_watch_list.front();
	}

	bool Poller::unwatch(
		detail::WatchEntry const * entry)
	{
		assert(entry != nullptr);
		assert(entry->socket_handle != -1);
#ifdef NETLIB_EPOLL
		// Is needed because in some kernel versions, the event pointer must not be null.
		static ::epoll_event event;

		// Return false if the socket was not watched.
		if(-1 == epoll_ctl(m_poller, EPOLL_CTL_DEL, entry->socket_handle, &event))
			return false;

		--m_event_list_size;
#else
		bool found = false;

		// Find the poll list entry of that socket.
		for(std::size_t index = 0; index < m_sockets.size(); index++)
			if(static_cast<::pollfd *>(m_poll_list)[index].fd == entry->socket_handle)
			{
				// Erase the entry from the poll list.
				std::size_t end = m_sockets.size()-1;
				for(std::size_t i = index; i < end; i++)
					static_cast<::pollfd *>(m_poll_list)[i] = static_cast<::pollfd *>(m_poll_list)[i+1];

				found = true;
				break;
			}

		// Return false if the socket was not watched.
		if(!found)
			return false;

		// Remove the socket from the look up.
		m_sockets.erase(m_sockets.find(entry->socket_handle));
#endif

		// Remove the watch list entry.
		m_watch_list.erase(entry->iterator);
		return true;
	}

	void Poller::unwatch_all()
	{
		m_watch_list.clear();
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

		events.reserve(events.size() + count);

		// Find the `count` events that were returned.
		for(std::size_t i = 0; i < count; i++)
		{
			::epoll_event & it = static_cast<::epoll_event *>(m_event_list)[i];

			if(it.events & (EPOLLIN | EPOLLOUT | EPOLLERR))
			{
				PollEvent event;
				event.entry = static_cast<detail::WatchEntry *>(it.data.ptr);
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

		events.reserve(events.size() + count);

		for(std::size_t i = 0; count; i++)
		{
			::pollfd & it = static_cast<::pollfd *>(m_poll_list)[i];
			if(it.revents & (POLLIN | POLLOUT | POLLERR))
			{
				--count;

				PollEvent event;
				event.entry = &*m_sockets[it.fd];
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