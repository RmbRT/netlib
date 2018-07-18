#ifndef __netlib_poller_hpp_defined
#define __netlib_poller_hpp_defined

#include "Socket.hpp"

#include <unordered_map>
#include <vector>

// `epoll` is only available on GNU/Linux.
#ifdef __unix__
#define NETLIB_EPOLL
#endif


namespace netlib
{
	/** Efficiently polls socket updates. */
	class Poller
	{
		/** The watched sockets. */
		std::unordered_map<std::uintptr_t, Socket *> m_sockets;
#ifdef NETLIB_EPOLL
		/** The poller object. */
		std::uintptr_t m_poller;
#else
		void * m_poll_list;
		std::size_t m_poll_list_size;
		std::size_t m_poll_list_capacity;
#endif
	public:
		/** Creates an empty poller. */
		Poller();

		/** Creates a poller that watches the requested sockets.
		@param[in] sockets:
			The sockets to watch.
		@param[in] count:
			The socket count. */
		explicit Poller(
			Socket * const * sockets,
			std::size_t count);

		~Poller();

		/** Watches sockets.
		@param[in] sockets:
			The sockets to watch.
		@param[in] count:
			The socket count. */
		bool watch(
			Socket * const * sockets,
			std::size_t count);

		bool watch(
			Socket * socket);

		/** Unwatches sockets.
		@param[in] sockets:
			The sockets to unwatch.
		@param[in] count:
			The socket count. */
		bool unwatch(
			Socket * const * sockets,
			std::size_t count);

		bool unwatch(
			Socket * socket);

		bool unwatch_all();

		/** Polls updated sockets.
			Waits until at least one socket has pending input, or the timeout expires.
		@param[out] pending:
			The sockets with pending input.
		@param[in] timeout:
			The timeout. */
		bool poll(
			std::vector<Socket *> &pending,
			std::size_t timeout = 0);

	private:
#ifndef NETLIB_EPOLL
		void reserve(
			std::size_t entries);
#endif

	};
}

#endif