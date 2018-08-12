#ifndef __netlib_async_receive_hpp_defined
#define __netlib_async_receive_hpp_defined

#include "../Socket.hpp"

#include <libcr/libcr.hpp>

namespace netlib::async
{
	/** Receives data asynchronously.
		Invoke this whenever the socket is able to receive data. */
	COROUTINE(Receive)
		/** Initialises the coroutine.
		@param[in] socket:
			The socket to receive data with.
		@param[in] data:
			The data to receive.
		@param[in] size:
			The byte count to receive. */
		Receive(
			StreamSocket &socket,
			void * data,
			std::size_t size);

		/** Whether an error occurred. */
		inline bool error() const;
	CR_STATE
		StreamSocket &socket;
		std::uint8_t * data;
		std::size_t size;

		bool m_error;
	CR_EXTERNAL
}

#include "Receive.inl"

#endif