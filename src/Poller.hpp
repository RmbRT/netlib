#ifndef __netlib_poller_hpp_defined
#define __netlib_poller_hpp_defined

#include "defines.hpp"
#include "Socket.hpp"

#include <unordered_map>
#include <vector>
#include <list>


// `epoll` is only available on GNU/Linux.
#ifdef __unix__
#define NETLIB_EPOLL
#endif


namespace netlib
{
	namespace x
	{
		class Connection;
		class ConnectionListener;
	}

	class Poller;

	namespace detail
	{
		struct WatchEntry
		{
			Socket * socket;
			std::list<WatchEntry>::iterator iterator;

			WatchEntry() = default;
			WatchEntry(
				socket_t socket_handle,
				Socket * socket);

			// Hide the socket handle.
		private:
			friend class ::netlib::Poller;
			socket_t socket_handle;
		};
	}


	/** Socket polling event type. */
	struct PollEvent
	{
		/** The watch entry. */
		detail::WatchEntry const * entry;
		/** Whether the socket has new data available for reading. */
		bool can_read;
		/** Whether the socket's output buffer is available. */
		bool can_write;
		/** Whether there was an error with the socket. */
		bool error;

		/** Handles the event. */
		bool operator()() const;
	};

	/** Efficiently polls socket updates.
		Uses `epoll()` when available, otherwise uses `poll()`. */
	class Poller
	{
		std::list<detail::WatchEntry> m_watch_list;
#ifdef NETLIB_EPOLL
		/** The poller object. */
		std::uintptr_t m_poller;
		/** The list of epoll events. */
		void * m_event_list;
		/** The list capacity. */
		std::size_t m_event_list_capacity;
		/** The list size. */
		std::size_t m_event_list_size;
#else
		/** The list of poll entries. */
		void * m_poll_list;
		/** The list capacity. */
		std::size_t m_poll_list_capacity;
		/** The watched sockets. */
		std::unordered_map<detail::socket_t, std::list<detail::WatchEntry>::iterator> m_sockets;
#endif
	public:
		/** Creates an empty poller. */
		Poller();

		/** Moves a poller instance.
		@param[in] move:
			The poller instance to move. */
		Poller(
			Poller && move);

		/** Moves a poller instance.
			The destination instance is destroyed.
		@param[in] move:
			The poller instance to move.
		@return
			The destination instance. */
		Poller &operator=(
			Poller && move);

		/** Destroys the poller and frees its resources. */
		~Poller();

		NETLIB_INL bool empty() const;

		/** Watches a single socket.
		@param[in] socket:
			The socket to watch.
		@param[in] read:
			Whether to listen for input events.
		@param[in] write:
			Whether to listen for output events.
		@return
			The watch entry that was added, or null on failure. */
		detail::WatchEntry const * watch(
			Socket * socket,
			bool read,
			bool write);

		template<
			class T,
			class = typename std::enable_if<std::is_base_of<Socket, T>::value>::type>
		NETLIB_INL bool watch(
			T * object,
			bool read,
			bool write);

		/** Unwatches a watched object.
		@param[in] entry:
			The object to unwatch.
		@return
			Whether it succeeded. */
		bool unwatch(
			detail::WatchEntry const * entry);

		/** Unwatches all sockets and frees all resources. */
		void unwatch_all();

		/** Polls updated sockets.
			Waits until at least one event occurs, or until the timeout expires.
		@param[out] events:
			A list of the polled events.
		@param[in] ms_timeout:
			The timeout in milliseconds.
			0 for non-blocking, -1 for infinite timeout.
		@return
			Whether it succeeded. */
		bool poll(
			std::vector<PollEvent> &events,
			std::size_t ms_timeout = 0);

		/** Reserves space for `size` sockets.
		@param[in] size:
			How many sockets to prepare for. */
		void reserve(
			std::size_t size);
		/** Reserves space for `size` additional sockets.
		@param[in] size:
			How many additional sockets to prepare for. */
		NETLIB_INL void reserve_additional(
			std::size_t size);
	};
}

#include "Poller.inl"

#endif