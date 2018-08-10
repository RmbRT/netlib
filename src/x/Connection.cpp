#include "Connection.hpp"
#include <utility>
#include <cstdio>

namespace netlib
{
	namespace x
	{
		bool Connection::receive_pending(
			std::vector<byte_t> & out,
			std::size_t max_count)
		{
			out.clear();
			// for thread safety.
			static thread_local std::vector<byte_t> buffer(4096);

			std::size_t count = receive_pending(buffer, max_count);

			if(!count)
				return false;

			out.insert(out.end(), buffer.begin(), buffer.begin() + count);

			return true;
		}

		std::size_t Connection::receive_pending(
			void * out,
			std::size_t max_count)
		{
			if(!async()
			&& !pending())
				return 0;

			return recv(out, max_count);
		}

		bool Connection::receive_all(
			void * out,
			std::size_t count)
		{
			std::size_t received = 0;

			while(received != count)
			{
				std::size_t recv_count = count - received;
				if(!(recv_count = recv(reinterpret_cast<std::uint8_t *>(out) + received, recv_count))
				&& !(async() && would_block()))
					return false;

				received += recv_count;
			}

			return true;
		}

		bool Connection::receive_all(
			std::vector<byte_t> & out,
			std::size_t count)
		{
			out.resize(count);
			std::size_t received = 0;

			while(received != count)
			{
				std::size_t recv_count = count - received;
				if(!(recv_count = recv(out.data() + received, recv_count))
				&& !(async() && would_block()))
					// error?
					return false;

				received += recv_count;
			}

			return true;
		}

		bool Connection::send_all(
			void const * data,
			std::size_t size)
		{
			byte_t const * to_send =
				static_cast<byte_t const *>(
					data);
			while(size)
			{
				std::size_t sent = StreamSocket::send(
					to_send,
					size);

				if(!sent
				&& !(async() && would_block()))
					return false;

				size -= sent;
				to_send += sent;
			}

			return true;
		}
	}
}