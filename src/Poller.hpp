#ifndef __netlib_poller_hpp_defined
#define __netlib_poller_hpp_defined

#include <unordered_map>
#include <vector>

// `epoll` is only available on GNU/Linux.
#ifdef __unix__
#define NETLIB_EPOLL
#endif


namespace netlib
{
	class Socket;

	/** Efficiently polls socket updates.
		Uses `epoll()` when available, otherwise uses `poll()`. */
	class Poller
	{
		/** The watched sockets. */
		std::unordered_map<std::uintptr_t, Socket *> m_sockets;
#ifdef NETLIB_EPOLL
		/** The poller object. */
		std::uintptr_t m_poller;
		/** The list of epoll events. */
		void * m_event_list;
		/** The list capacity. */
		std::size_t m_event_list_capacity;
#else
		/** The list of poll entries. */
		void * m_poll_list;
		/** The list capacity. */
		std::size_t m_poll_list_capacity;
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

		/** Creates a poller that watches the requested sockets.
		@param[in] sockets:
			The sockets to watch.
		@param[in] count:
			The socket count. */
		explicit Poller(
			Socket * const * sockets,
			std::size_t count);

		/** Destroys the poller and frees its resources. */
		~Poller();

		/** Returns the number of watched sockets. */
		inline std::size_t watched() const;

		/** Detects whether the poller is watching a socket.
		@param[in] socket:
			The socket to check for.
		@return
			Whether the socket is being watched. */
		bool watching(
			Socket * socket) const;

		/** Watches sockets.
		@param[in] sockets:
			The sockets to watch.
		@param[in] count:
			The socket count.
		@return
			Whether it succeeded. */
		bool watch(
			Socket * const * sockets,
			std::size_t count);

		/** Watches a single socket.
		@param[in] socket:
			The socket to watch.
		@return
			Whether it succeeded. */
		bool watch(
			Socket * socket);

		/** Unwatches sockets.
		@param[in] sockets:
			The sockets to unwatch.
		@param[in] count:
			The socket count.
		@return
			Whether it succeeded. */
		bool unwatch(
			Socket * const * sockets,
			std::size_t count);

		/** Unwatches a single socket.
		@param[in] socket:
			The socket to unwatch.
		@return
			Whether it succeeded. */
		bool unwatch(
			Socket * socket);

		/** Unwatches all sockets and frees all resources. */
		void unwatch_all();

		/** Polls updated sockets.
			Waits until at least one socket has pending input, or the timeout expires.
		@param[out] pending:
			The sockets with pending input.
		@param[in] ms_timeout:
			The timeout in milliseconds.
			0 for non-blocking, -1 for infinite timeout.
		@return
			Whether it succeeded. */
		bool poll(
			std::vector<Socket *> &pending,
			std::size_t ms_timeout = 0);

		/** Reserves space for `size` sockets.
		@param[in] size:
			How many sockets to prepare for. */
		void reserve(
			std::size_t size);
	};
}

#include "Poller.inl"

#endif