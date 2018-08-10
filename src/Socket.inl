#ifndef __netlib_socket_inl_defined
#define __netlib_socket_inl_defined

namespace netlib
{
	NETLIB_INL bool Socket::async() const { return m_async; }
	NETLIB_INL bool Socket::exists() const { return m_socket != -1; }
	NETLIB_INL Socket::operator bool() const { return exists(); }
	NETLIB_INL SocketAddress const& Socket::address() const { return m_address; }
	NETLIB_INL Protocol Socket::protocol() const { return m_protocol; }
	NETLIB_INL SocketType Socket::type() const { return m_type; }
}

#endif