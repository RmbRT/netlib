#include "Receive.hpp"

#include <cassert>

namespace netlib::async
{
	void Receive::prepare(
		StreamSocket &socket,
		void * data,
		std::size_t size)
	{
		Base::prepare();
		this->socket = &socket;
		this->data = reinterpret_cast<std::uint8_t *>(data);
		this->size = size;
		m_error = false;
	}

	void Receive::prepare(
		StreamSocket &socket,
		void * data,
		std::size_t size,
		cr::Coroutine * parent)
	{
		Base::prepare(parent);
		this->socket = &socket;
		this->data = reinterpret_cast<std::uint8_t *>(data);
		this->size = size;
		m_error = false;
	}

	CR_IMPL(Receive)
		while(size)
		{
			// Make sure this coroutine is used as intended.
			assert(socket->async() || socket->pending());

			// Need a separate scope for the local variable.
			{
				std::size_t count = socket->recv(data, size);
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

	void BufferedReceive::prepare(
		x::BufferedConnection &connection,
		void * data,
		std::size_t size)
	{
		Base::prepare();
		this->connection = &connection;
		this->data = reinterpret_cast<std::uint8_t *>(data);
		this->size = size;
	}

	void BufferedReceive::prepare(
		x::BufferedConnection &connection,
		void * data,
		std::size_t size,
		cr::Coroutine * parent)
	{
		Base::prepare(parent);
		this->connection = &connection;
		this->data = reinterpret_cast<std::uint8_t *>(data);
		this->size = size;
	}

	CR_IMPL(BufferedReceive)
		while(size)
		{
			// Make sure this coroutine is used as intended.
			assert(!connection->input().empty());

			// Need a separate scope for the local variable.
			{
				std::size_t count = connection->receive(data, size);
				size -= count;
				data += count;
			}

			// Only yield if there is still more data to come.
			if(size)
				CR_YIELD;
		}
	CR_IMPL_END
}