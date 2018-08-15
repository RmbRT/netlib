#include "Send.hpp"
#include <cassert>

namespace netlib::async
{
	void Send::prepare(
		StreamSocket &socket,
		void const * data,
		std::size_t size)
	{
		this->socket = &socket;
		this->data = reinterpret_cast<std::uint8_t const *>(data);
		this->size = size;
		m_error = false;
	}

	CR_IMPL_BEGIN(Send)
		while(size)
		{
			// Need a separate scope for the local variable.
			{
				std::size_t count = socket->send(data, size);
				size -= count;
				data += count;

				if(!count
				&& !(socket->async() && Socket::would_block()))
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

	void BufferedSend::prepare(
		x::BufferedConnection &connection,
		void const * data,
		std::size_t size)
	{
		this->connection = &connection;
		this->data = reinterpret_cast<std::uint8_t const *>(data);
		this->size = size;
	}

	CR_IMPL_BEGIN(BufferedSend)
		while(size)
		{
			// Make sure this coroutine is used as intended.
			assert(!connection->output().full());

			// Need a separate scope for the local variable.
			{
				std::size_t count = connection->send(data, size);
				size -= count;
				data += count;
			}

			// Only yield if there is still more data to come.
			if(size)
				CR_YIELD;
		}
	CR_IMPL_END
}