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
		if(m_output.empty())
			return true;

		std::size_t received;
		if(Status::kSuccess == StreamSocket::send(
			m_output.data(),
			m_output.continuous_data(),
			received))
		{
			m_output.remove(received);
			return true;
		}

		return false;
	}

	bool BufferedConnection::receive_some()
	{
		if(m_input.full())
			return true;

		std::size_t sent;
		if(Status::kSuccess == StreamSocket::recv(
			m_input.end(),
			m_input.continuous_free_space(),
			sent))
		{
			m_input.add(sent);
			return true;
		}

		return false;
	}

	void BufferedConnection::discard()
	{
		m_input.clear();
		m_output.clear();
	}

	CR_IMPL(BufferedConnection::Flush)
	CR_FINALLY
		while(!conn->m_output.empty())
		{
			CR_AWAIT(conn->Socket::m_output.wait());
			if(!conn->flush_some())
				CR_THROW;
		}
	CR_IMPL_END

	CR_IMPL(BufferedConnection::Send)
		while(size)
		{
			if(!conn->m_output.full())
			{
				std::size_t sent = conn->m_output.append(data, size);
				reinterpret_cast<std::uintptr_t &>(data) += sent;
				size -= sent;
			} else {
				CR_AWAIT(conn->Socket::m_output.wait());
				if(!conn->flush_some())
					CR_THROW;
			}
		}
	CR_FINALLY
	CR_IMPL_END

	CR_IMPL(BufferedConnection::Receive)
		while(size)
		{
			if(!conn->m_input.empty())
			{
				std::size_t received = conn->m_input.consume(data, size);
				reinterpret_cast<std::uintptr_t &>(data) += received;
				size -= received;
			} else {
				CR_AWAIT(conn->Socket::m_input.wait());
				if(!conn->receive_some())
					CR_THROW;
			}
		}
	CR_FINALLY
	CR_IMPL_END

	void BufferedConnection::close()
	{
		assert(m_input.empty());
		assert(m_output.empty());

		if(exists())
			Socket::shutdown(Shutdown::kBoth);
		Socket::close();
	}

	CR_IMPL(BufferedConnection::Connect)
		assert(!conn->exists());

		new (conn) StreamSocket(
			address.family);

		switch(conn->StreamSocket::connect(address))
		{
		case Status::kSuccess:
			CR_RETURN;
		default:
			CR_THROW;
		case Status::kInProgress:;
		}

		CR_AWAIT(conn->Socket::m_output.wait());
		CR_RETURN;
	CR_FINALLY
	CR_IMPL_END
}