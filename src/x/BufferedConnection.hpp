#ifndef __netlib_x_bufferedconnection_hpp_defined
#define __netlib_x_bufferedconnection_hpp_defined

#include "Connection.hpp"
#include "../util/Buffer.hpp"

#include <vector>
#include <cinttypes>

namespace netlib::x
{
	/** Buffered connection for increased efficiency.
		Uses fixed-sized buffers, to prevent the overhead of allocating new buffers for every request, so the underlying socket limits the amount of operations that can be performed by this connection. This means that an additional buffering / job queue layer is needed to achieve variable-sized buffering. */
	class BufferedConnection : protected StreamSocket
	{
		/** The input buffer. */
		util::Buffer m_input;
		/** The output buffer. */
		util::Buffer m_output;
		bool m_should_flush;
	public:
		using StreamSocket::operator bool;
		using StreamSocket::exists;
		using StreamSocket::connect;
		using StreamSocket::async;
		using StreamSocket::set_async;

		BufferedConnection(
			std::size_t input_buffer,
			std::size_t output_buffer);
		explicit BufferedConnection(
			std::size_t buffer_size);

		inline util::Buffer const& input() const noexcept;
		inline util::Buffer const& output() const noexcept;

		/** Flushes the output buffer / or part of it.
		@return
			Whether the operation succeeded. */
		bool flush_some();
		/** Attempts to fill the input buffer. */
		bool receive_some();

		/** Puts data into the send buffer.
		@param[in] data:
			The data to send.
		@param[in] size:
			The size of the data.
		@return
			How many bytes were actually buffered. */
		std::size_t send(
			void const * data,
			std::size_t size);

		/** Receives data from the input buffer.
		@param[out] data:
			The data to receive.
		@param[in] size:
			How many bytes to receive.
		@return
			How many bytes were actually received. */
		std::size_t receive(
			void * data,
			std::size_t size);

		void close();
		void shutdown(
			Shutdown what);
	};
}

#include "BufferedConnection.inl"

#endif