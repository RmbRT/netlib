#include "Receive.hpp"

namespace netlib::async
{
	Receive::Receive(
		x::Connection & connection,
		void * data,
		std::size_t size):
		connection(connection),
		data(reinterpret_cast<std::uint8_t *>(data)),
		size(size),
		m_error(false)
	{
	}

	CR_IMPL_BEGIN(Receive)
		while(size)
		{
			// Need a separate scope for the local variable.
			{
				std::size_t count = connection.receive_pending(data, size);
				size -= count;
				data += count;

				if(!count
				&& !(connection.async() && Socket::would_block()))
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
}