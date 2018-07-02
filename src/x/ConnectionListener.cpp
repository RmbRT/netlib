#include "ConnectionListener.hpp"
#include <utility>
#include <cassert>

namespace netlib
{
	namespace x
	{
		ConnectionListener::ConnectionListener():
			StreamSocket(),
			m_listening(false)
		{
		}

		ConnectionListener::ConnectionListener(
			SocketAddress const& listen_addr):
			StreamSocket(),
			m_listening(false)
		{
			listen(listen_addr);
		}

		ConnectionListener::ConnectionListener(
			ConnectionListener && move):
			StreamSocket(std::move(move)),
			m_listening(move.m_listening)
		{
			move.m_listening = false;
		}

		ConnectionListener &ConnectionListener::operator=(
			ConnectionListener && move)
		{
			if(this == &move)
				return *this;

			close();

			*(StreamSocket *)this = std::move(move);

			m_listening = move.m_listening;
			move.m_listening = false;

			return *this;
		}

		bool ConnectionListener::listen(
			SocketAddress const& listen_addr,
			bool reuse_address)
		{
			unlisten();

			if(!exists())
				*(StreamSocket *)this = StreamSocket(listen_addr.family);

			return m_listening = exists()
				&& bind(listen_addr, reuse_address)
				&& StreamSocket::listen();
		}

		void ConnectionListener::unlisten()
		{
			// Call shutdown() to break blocking accept() calls.
			if(exists())
				shutdown(Shutdown::kBoth);

			close();
			m_listening = false;
		}

		Connection ConnectionListener::accept()
		{
			StreamSocket incoming;
			if(!static_cast<StreamSocket *>(this)->accept(incoming))
				return Connection();
			else
				return Connection(std::move(incoming));
		}
	}
}