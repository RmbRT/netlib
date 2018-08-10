#include "Send.hpp"

namespace netlib::async
{
	Send::Send(
		x::Connection & connection,
		void const * data,
		std::size_t size):
		connection(connection),
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
				std::size_t count = connection.send(data, size);
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