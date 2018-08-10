#ifndef __netlib_async_send_hpp_defined
#define __netlib_async_send_hpp_defined

#include "../x/Connection.hpp"

#include <libcr/libcr.hpp>

namespace netlib::async
{
	/** Sends data asynchronously.
		Invoke this whenever the connection is able to send data. */
	COROUTINE(Send)
		/** Initialises the coroutine.
		@param[in] connection:
			The connection to send data with.
		@param[in] data:
			The data to send.
		@param[in] size:
			The byte count to receive. */
		Send(
			x::Connection &connection,
			void const * data,
			std::size_t size);

		/** Whether an error occurred. */
		inline bool error() const;
	CR_STATE
		x::Connection &connection;
		std::uint8_t const * data;
		std::size_t size;

		bool m_error;
	CR_EXTERNAL
}

#include "Send.inl"

#endif