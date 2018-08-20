#include "BufferedConnection.hpp"
#include <cassert>

namespace netlib::x
{
	BufferedConnection::BufferedConnection(
		std::size_t input_buffer,
		std::size_t output_buffer):
		m_input(input_buffer),
		m_output(output_buffer)
	{
	}

	BufferedConnection::BufferedConnection(
		std::size_t buffer_size):
		m_input(buffer_size),
		m_output(buffer_size)
	{
	}

	BufferedConnection::BufferedConnection(
		StreamSocket && socket,
		std::size_t input_buffer,
		std::size_t output_buffer):
		StreamSocket(std::move(socket)),
		m_input(input_buffer),
		m_output(output_buffer)
	{
	}

	BufferedConnection::BufferedConnection(
		StreamSocket && socket,
		std::size_t buffer_size):
		StreamSocket(std::move(socket)),
		m_input(buffer_size),
		m_output(buffer_size)
	{
	}

	BufferedConnection::~BufferedConnection()
	{
		close();
	}

	bool BufferedConnection::flush_some()
	{
		return m_output.empty()
			|| m_output.remove(
				StreamSocket::send(
					m_output.data(),
					m_output.continuous_data()))
			|| (async() && Socket::would_block());
	}

	bool BufferedConnection::receive_some()
	{
		return m_input.full()
			|| m_input.add(
				StreamSocket::recv(
					m_input.end(),
					m_input.continuous_free_space()))
			|| (async() && Socket::would_block());
	}

	void BufferedConnection::discard()
	{
		m_input.clear();
		m_output.clear();
	}

	void BufferedConnection::close()
	{
		assert(m_input.empty());
		assert(m_output.empty());

		if(exists())
			Socket::shutdown(Shutdown::kBoth);
		Socket::close();
	}

	bool BufferedConnection::connect(
		SocketAddress const& address)
	{
		assert(!exists());

		new (this) StreamSocket(
			address.family);

		return StreamSocket::connect(address);
	}

	bool BufferedConnection::connect(
		async_t,
		SocketAddress const& address)
	{
		assert(!exists());

		new (this) StreamSocket(
			kAsync,
			address.family);

		return StreamSocket::connect(address);
	}
}