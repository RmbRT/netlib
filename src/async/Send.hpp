#ifndef __netlib_async_send_hpp_defined
#define __netlib_async_send_hpp_defined

#include "../Socket.hpp"
#include "../x/BufferedConnection.hpp"

#include <libcr/libcr.hpp>

namespace netlib::async
{
	/** Sends data asynchronously.
		Invoke this whenever the socket is able to send data. */
	COROUTINE(Send)
		/** Initialises the coroutine.
		@param[in] socket:
			The socket to send data with.
		@param[in] data:
			The data to send.
		@param[in] size:
			The byte count to send. */
		void prepare(
			StreamSocket &socket,
			void const * data,
			std::size_t size);

		/** Initialises the coroutine.
		@param[in] socket:
			The socket to send data with.
		@param[in] data:
			The data to send.
		@param[in] size:
			The byte count to send.
		@param[in] parent:
			The parent coroutine. */
		void prepare(
			StreamSocket &socket,
			void const * data,
			std::size_t size,
			cr::Coroutine * parent);

		/** Whether an error occurred. */
		inline bool error() const;
	CR_STATE
		StreamSocket * socket;
		std::uint8_t const * data;
		std::size_t size;

		bool m_error;
	CR_EXTERNAL


	/** Sends data asynchronously to a buffered connection.
		Invoke this whenever the buffered connection is able to send data. */
	COROUTINE(BufferedSend)
		/** Initialises the coroutine.
		@param[in] connection:
			The connection to send data with.
		@param[in] data:
			The data to send.
		@param[in] size:
			The byte count to send. */
		void prepare(
			x::BufferedConnection &connection,
			void const * data,
			std::size_t size);

		/** Initialises the coroutine.
		@param[in] connection:
			The connection to send data with.
		@param[in] data:
			The data to send.
		@param[in] size:
			The byte count to send.
		@param[in] parent:
			The parent coroutine. */
		void prepare(
			x::BufferedConnection &connection,
			void const * data,
			std::size_t size,
			cr::Coroutine * parent);
	CR_STATE
		x::BufferedConnection * connection;
		std::uint8_t const * data;
		std::size_t size;
	CR_EXTERNAL
}

#include "Send.inl"

#endif