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

			while(pending() && out.size() < max_count)
			{

				std::size_t recv_count = (out.size() + buffer.size() > max_count)
					? max_count - out.size()
					: buffer.size();
				std::size_t count = recv(buffer.data(), recv_count);
				// error?
				if(!count)
					return false;

				out.insert(out.end(), buffer.begin(), buffer.begin() + count);
			}

			return true;
		}

		bool Connection::receive(
			void * out,
			std::size_t count)
		{
			std::size_t received = 0;

			while(received != count)
			{
				std::size_t recv_count = count - received;
				if(!(recv_count = recv(reinterpret_cast<std::uint8_t *>(out) + received, recv_count)))
					return false;

				received += recv_count;
			}

			return true;
		}

		bool Connection::receive(
			std::vector<byte_t> & out,
			std::size_t count)
		{
			out.resize(count);
			std::size_t received = 0;

			while(received != count)
			{
				std::size_t recv_count = count - received;
				if(!(recv_count = recv(out.data() + received, recv_count)))
					// error?
					return false;

				received += recv_count;
			}

			return true;
		}

		bool Connection::send(
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
				if(!sent)
					return false;
				size -= sent;
				to_send += sent;
			}

			return true;
		}
	}
}