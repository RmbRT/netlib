#ifndef __netlib_x_bufferedconnection_hpp_defined
#define __netlib_x_bufferedconnection_hpp_defined

#include "../Socket.hpp"
#include "../util/Buffer.hpp"

#include <libcr/primitives.hpp>

#include <vector>
#include <cinttypes>

namespace netlib::x
{
	/** Buffered connection for increased efficiency.
		Uses fixed-sized buffers, to prevent the overhead of allocating new buffers for every request, so the underlying socket limits the amount of operations that can be performed by this connection. This means that an additional buffering / job queue layer is needed to achieve variable-sized buffering.

		This class is best used in async/nonblocking mode in conjunction with polling and coroutines. */
	class BufferedConnection : protected StreamSocket
	{
		friend class ::netlib::Poller;
		/** The input buffer. */
		util::Buffer m_input;
		/** The output buffer. */
		util::Buffer m_output;
	public:
		using StreamSocket::operator bool;
		using StreamSocket::exists;
		using StreamSocket::connect;

		BufferedConnection(BufferedConnection&&) = default;
		BufferedConnection &operator=(
			BufferedConnection &&) = default;

		static constexpr BufferedConnection * cast_from_base(
			Socket * socket);
		static constexpr BufferedConnection const * cast_from_base(
			Socket const * socket);

		/** Creates a closed buffered connection.
		@param[in] family:
			The address family to use.
		@param[in] input_buffer:
			The input buffer size, in bytes.
		@param[in] output_buffer:
			The output buffer size, in bytes. */
		BufferedConnection(
			std::size_t input_buffer,
			std::size_t output_buffer);

		/** Creates a closed buffered connection.
		@param[in] buffer_size:
			The input and output buffers' size, in bytes. */
		explicit BufferedConnection(
			std::size_t buffer_size);

		/** Creates a buffered connection from an existing socket.
		@param[in] socket:
			The existing socket.
		@param[in] input_buffer:
			The input buffer size, in bytes.
		@param[in] output_buffer:
			The output buffer size, in bytes. */
		BufferedConnection(
			StreamSocket && socket,
			std::size_t input_buffer,
			std::size_t output_buffer);

		/** Creates a buffered connection from an existing socket.
		@param[in] socket:
			The existing socket.
		@param[in] buffer_size:
			The input and output buffers' size, in bytes. */
		BufferedConnection(
			StreamSocket && socket,
			std::size_t buffer_size);

		~BufferedConnection();

		/** Returns the input buffer. */
		inline util::Buffer const& input() const noexcept;
		/** Returns the output buffer. */
		inline util::Buffer const& output() const noexcept;

		/** Flushes the output buffer / or part of it.
		@return
			Whether the operation succeeded. */
		bool flush_some();
		/** Attempts to fill the input buffer.
		@return
			Whether the operation succeeded. */
		bool receive_some();

		/** Discards all buffered input and output. */
		void discard();

		/** Flushes all buffered data to be sent. */
		COROUTINE(Flush, void)
		CR_STATE(
			(BufferedConnection *) conn)
		CR_EXTERNAL

		/** Buffers, and potentially flushes, data to be sent. */
		COROUTINE(Send, void)
		CR_STATE(
			(BufferedConnection *) conn,
			(void const *) data,
			(std::size_t) size)
		CR_EXTERNAL

		/** Receives and buffers data. */
		COROUTINE(Receive, void)
		CR_STATE(
			(BufferedConnection *) conn,
			(void *) data,
			(std::size_t) size)
		CR_EXTERNAL

		/** Closes the connection.
			This must only be called if there is no buffered input or output. */
		void close();

		/** Connects to the desired address. */
		COROUTINE(Connect, void)
		CR_STATE(
			(BufferedConnection *) conn,
			(SocketAddress const&) address)
		CR_EXTERNAL
	};
}

#include "BufferedConnection.inl"

#endif