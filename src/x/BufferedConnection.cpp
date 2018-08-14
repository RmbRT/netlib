#include "BufferedConnection.hpp"

namespace netlib::x
{
	BufferedConnection::BufferedConnection(
		std::size_t input_buffer,
		std::size_t output_buffer):
		m_input(input_buffer),
		m_output(output_buffer)
	{
	}

	bool BufferedConnection::flush_some()
	{
		return !(m_output.empty()
			|| m_output.remove(
				StreamSocket::send(
					m_output.data(),
					m_output.continuous_data()))
			|| (async() && Socket::would_block()));
	}

	bool BufferedConnection::receive_some()
	{
		return !(!m_input.full()
			|| m_input.add(
				StreamSocket::recv(
					m_input.end(),
					m_input.free_space()))
			|| (async() && Socket::would_block()));
	}
}