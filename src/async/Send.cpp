#include "Send.hpp"
#include <cassert>

namespace netlib::async
{
	Send::Send(
		StreamSocket & socket,
		void const * data,
		std::size_t size):
		socket(socket),
		data(reinterpret_cast<std::uint8_t const *>(data)),
		size(size),
		m_error(false)
	{
	}

	CR_IMPL_BEGIN(Send)
		while(size)
		{
			// Need a separate scope for the local variable.
			{
				std::size_t count = socket.send(data, size);
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

	BufferedSend::BufferedSend(
		x::BufferedConnection &connection,
		void const * data,
		std::size_t size):
		connection(connection),
		data(reinterpret_cast<std::uint8_t const *>(data)),
		size(size)
	{
	}

	CR_IMPL_BEGIN(BufferedSend)
		while(size)
		{
			// Make sure this coroutine is used as intended.
			assert(!connection.output().full());

			// Need a separate scope for the local variable.
			{
				std::size_t count = connection.send(data, size);
				size -= count;
				data += count;
			}

			// Only yield if there is still more data to come.
			if(size)
				CR_YIELD;
		}
	CR_IMPL_END
}