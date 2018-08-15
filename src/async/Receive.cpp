#include "Receive.hpp"

#include <cassert>

namespace netlib::async
{
	Receive::Receive(
		StreamSocket &socket,
		void * data,
		std::size_t size):
		socket(socket),
		data(reinterpret_cast<std::uint8_t *>(data)),
		size(size),
		m_error(false)
	{
	}

	CR_IMPL_BEGIN(Receive)
		while(size)
		{
			// Make sure this coroutine is used as intended.
			assert(socket.async() || socket.pending());

			// Need a separate scope for the local variable.
			{
				std::size_t count = socket.recv(data, size);
				size -= count;
				data += count;

				if(!count
				&& !(socket.async() && Socket::would_block()))
				{
					m_error = true;
					break;
				}
			}

			// Only yield if there is still more data to come.
			if(size)
				CR_YIELD;
		}
	CR_IMPL_END

	BufferedReceive::BufferedReceive(
		x::BufferedConnection &connection,
		void * data,
		std::size_t size):
		connection(connection),
		data(reinterpret_cast<std::uint8_t *>(data)),
		size(size)
	{
	}

	CR_IMPL_BEGIN(BufferedReceive)
		while(size)
		{
			// Make sure this coroutine is used as intended.
			assert(!connection.input().empty());

			// Need a separate scope for the local variable.
			{
				std::size_t count = connection.receive(data, size);
				size -= count;
				data += count;
			}

			// Only yield if there is still more data to come.
			if(size)
				CR_YIELD;
		}
	CR_IMPL_END
}