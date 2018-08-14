#include "Socket.hpp"
#include "internal/platform.hpp"
#include "internal/to_socket_address.hpp"
#include "Runtime.hpp"

#include <cassert>
#include <type_traits>
#include <cstring>

#include <poll.h>
#include <fcntl.h>

#ifndef SOCKET_ERROR
int const SOCKET_ERROR = -1;
#endif

namespace netlib
{
	::sockaddr_storage to_sockaddr_storage(
		SocketAddress const& addr)
	{
		::sockaddr_storage ret;
		memset(&ret, 0, sizeof(ret));

		ret.ss_family = to_native_api(addr.family);

		switch(addr.family)
		{
		case AddressFamily::kIPv4:
			{
				::sockaddr_in & sin = reinterpret_cast<::sockaddr_in &>(ret);
				IPv4SocketAddress const& ipv4 = addr.address.ipv4;

				// set fields.
				sin.sin_port = TO_NET_PORT(ipv4.port);
				// no need to cast, since the byte array is in the right order already.
				sin.sin_addr.s_addr = reinterpret_cast<std::uint32_t const&>(ipv4.address);
			} break;
		case AddressFamily::kIPv6:
			{
				::sockaddr_in6 & sin6 = reinterpret_cast<::sockaddr_in6 &>(ret);
				IPv6SocketAddress const& ipv6 = addr.address.ipv6;

				// set fields.
				sin6.sin6_port = TO_NET_PORT(ipv6.port);
				sin6.sin6_flowinfo = TO_NET_FLOWINFO(ipv6.field);

				std::uint16_t * const words = reinterpret_cast<std::uint16_t *>(
					&sin6.sin6_addr.s6_addr[0]);

				// convert 2-Byte pairs to the right byte order.
				for(std::size_t i = 8; i--;)
					words[i] = TO_NET_IPV6DATA((&ipv6.address.d0)[i]);

				sin6.sin6_scope_id = TO_NET_SCOPEID(ipv6.scope);
			} break;
		default:
			{
				assert(!"invalid address family");
			} break;
		};

		return ret;
	}


	int to_native_api(
		SocketType type)
	{
		static int const k_lookup[] = {
			SOCK_STREAM,
			SOCK_DGRAM,
			SOCK_RAW,
			SOCK_RDM,
			SOCK_SEQPACKET
		};

		static_assert(
			_countof(k_lookup) == NETLIB_COUNT(SocketType),
			"Invalid lookup table size.");

		assert((std::size_t)type < NETLIB_COUNT(SocketType));

		return k_lookup[(std::size_t) type];
	}


	Socket::Socket(
		SocketAddress const& address,
		SocketType type,
		Protocol protocol):
		m_address(address),
		m_type(type),
		m_protocol(protocol),
		m_socket(),
		m_async(false)
	{
		assert(Runtime::exists());

		m_socket = ::socket(
			to_native_api(address.family),
			to_native_api(type),
			to_native_api(protocol));
	}

	Socket::Socket(
		async_t,
		SocketAddress const& address,
		SocketType type,
		Protocol protocol):
		m_address(address),
		m_type(type),
		m_protocol(protocol),
		m_socket(),
		m_async()
	{
		assert(Runtime::exists());

// Create nonblocking socket atomically, if possible.
#ifdef __unix__
		m_socket = ::socket(
			to_native_api(address.family),
			to_native_api(type) | SOCK_NONBLOCK,
			to_native_api(protocol));

		m_async = true;
#else
		m_socket = ::socket(
			to_native_api(address.family),
			to_native_api(type)),
			to_native_api(protocol));

		if(exists())
		{
			m_async = false;
			set_async(true);
		}
#endif
	}

	Socket::Socket(
		AddressFamily family,
		SocketType type,
		Protocol protocol):
		m_address(family),
		m_type(type),
		m_protocol(protocol),
		m_socket(-1),
		m_async(false)
	{
		assert(Runtime::exists());

		m_socket = ::socket(
			to_native_api(m_address.family),
			to_native_api(type),
			to_native_api(protocol));
	}

	Socket::Socket(
		async_t,
		AddressFamily family,
		SocketType type,
		Protocol protocol):
		m_address(family),
		m_type(type),
		m_protocol(protocol),
		m_socket(-1),
		m_async(false)
	{
		assert(Runtime::exists());

// Create nonblocking socket atomically, if possible.
#ifdef __unix__
		m_socket = ::socket(
			to_native_api(m_address.family),
			to_native_api(type) | SOCK_NONBLOCK,
			to_native_api(protocol));

		m_async = true;
#else
		m_socket = ::socket(
			to_native_api(m_address.family),
			to_native_api(type)),
			to_native_api(protocol));

		if(exists())
		{
			m_async = false;
			set_async(true);
		}
#endif
	}

	Socket::Socket():
		m_address(),
		m_type(),
		m_protocol(),
		m_socket(-1),
		m_async()
	{
	}

	Socket::~Socket()
	{
		close();
	}

	void Socket::close()
	{
		if(exists())
		{
			assert(Runtime::exists());

#ifdef NETLIB_WINDOWS
			::closesocket
#else
			::close
#endif
			(m_socket);
		}

		m_socket = -1;
	}

#ifndef NETLIB_WINDOWS
#define SD_RECEIVE SHUT_RD
#define SD_SEND SHUT_WR
#define SD_BOTH SHUT_RDWR
#endif

	bool Socket::shutdown(Shutdown param)
	{
		assert(Runtime::exists());
		assert(exists());

		static int const parameter[] = {
			SHUT_RD,
			SHUT_WR,
			SHUT_RDWR
		};
		assert(std::size_t(param) < _countof(parameter));

		return !::shutdown(m_socket, parameter[std::size_t(param)]);
	}

	bool Socket::set_async(
		bool async)
	{
		assert(Runtime::exists());
		assert(exists());
		if(async != m_async)
		{
#ifdef NETLIB_WINDOWS
			unsigned long mode = async ? 1 : 0;
			if(ioctlsocket(m_socket, FIONBIO, &mode))
				return false;
#else
			int flags = fcntl(m_socket, F_GETFL, 0);
			if(async)
				flags |= O_NONBLOCK;
			else
				flags &= ~O_NONBLOCK;

			if(fcntl(m_socket, F_SETFL, flags))
				return false;
#endif
			m_async = async;
		}
		return true;
	}

	bool Socket::error()
	{
		assert(Runtime::exists());
		assert(exists());

		socklen_t size = sizeof(int);
		int value;
		::getsockopt(
			m_socket,
			SOL_SOCKET,
			SO_ERROR,
			&value,
			&size);

		return value != 0;
	}

	bool Socket::would_block()
	{
		return errno == EWOULDBLOCK
			|| errno == EAGAIN;
	}

	size_t StreamSocket::send(
		const void* data,
		size_t size)
	{
		assert(Runtime::exists());
		assert(exists());

		assert(m_protocol == Protocol::kTCP || (m_protocol == Protocol::kDefault && m_type == SocketType::kStream));


		std::size_t result = ::send(m_socket, (const char*)data, size, 0);
		if(result == -1)
			return 0;
		else
			return result;
	}

	size_t DatagramSocket::sendto(
		void const * data,
		size_t size,
		SocketAddress const& to,
		size_t flags)
	{
		assert(Runtime::exists());
		assert(exists());

		assert(m_protocol == Protocol::kUDP || (m_protocol == Protocol::kDefault && m_type == SocketType::kDatagram));

		::sockaddr_storage addr = to_sockaddr_storage(m_address);
		std::size_t result = ::sendto(
			m_socket,
			(const char*)data,
			size,
			flags,
			reinterpret_cast<::sockaddr const *>(&addr),
			sizeof(::sockaddr));
		if(result == -1)
			return 0;
		else
			return result;
	}
	size_t StreamSocket::recv(
		void* data,
		size_t size)
	{
		assert(Runtime::exists());
		assert(exists());

		assert(m_protocol == Protocol::kTCP || (m_protocol == Protocol::kDefault && m_type == SocketType::kStream));

		std::size_t result = ::recv(
			m_socket,
			(char*)data,
			size,
			0);
		if(result == -1)
			return 0;
		else
			return result;
	}
	size_t DatagramSocket::recvfrom(
		void * data,
		size_t size,
		SocketAddress const& from,
		size_t flags)
	{
		assert(Runtime::exists());
		assert(exists());

		assert(m_protocol == Protocol::kUDP || (m_protocol == Protocol::kDefault && m_type == SocketType::kDatagram));

		::sockaddr_storage addr = to_sockaddr_storage(m_address);

		::socklen_t len = sizeof(::sockaddr);

		std::size_t result = ::recvfrom(
			m_socket,
			(char*)data,
			size,
			flags,
			reinterpret_cast<::sockaddr*>(&addr),
			&len);
		if(result == -1)
			return 0;
		else
			return result;
	}

	bool StreamSocket::connect(
		SocketAddress const& serverAddress)
	{
		assert(Runtime::exists());
		assert(exists());

		assert(m_protocol == Protocol::kTCP || (m_protocol == Protocol::kDefault && m_type == SocketType::kStream));

		m_address = serverAddress;
		::sockaddr_storage addr = to_sockaddr_storage(m_address);

		return SOCKET_ERROR != ::connect(
			m_socket,
			reinterpret_cast<::sockaddr const *>(&addr),
			sizeof(::sockaddr));
	}

	bool StreamSocket::bind(
		SocketAddress const& address,
		bool reuse_address)
	{
		assert(Runtime::exists());
		assert(exists());

		if(reuse_address)
		{
			static int const enable = 1;
			if(SOCKET_ERROR == ::setsockopt(
				m_socket,
				SOL_SOCKET,
				SO_REUSEADDR,
				&enable,
				sizeof(enable)))
				return false;
		}

		m_address = address;
		::sockaddr_storage addr = to_sockaddr_storage(m_address);
		return SOCKET_ERROR != ::bind(
			m_socket,
			reinterpret_cast<::sockaddr const *>(&addr),
			sizeof(::sockaddr));
	}
	bool StreamSocket::accept(
		StreamSocket &socket)
	{
		assert(Runtime::exists());
		assert(exists());

		sockaddr_in addr;
		socklen_t len = sizeof(::sockaddr);
		int sockid = ::accept(m_socket, reinterpret_cast<::sockaddr*>(&addr), &len);

		if(sockid == -1)
			return false;

		if(socket)
			socket.close();

		socket.m_address = to_socket_address(
			reinterpret_cast<::sockaddr const&>(addr));

		socket.m_protocol = m_protocol;
		socket.m_type = m_type;
		socket.m_socket = sockid;
		socket.m_async = false;

		return true;
	}

	bool StreamSocket::accept(
		async_t,
		StreamSocket &socket)
	{
		assert(Runtime::exists());
		assert(exists());

		sockaddr_in addr;
		socklen_t len = sizeof(::sockaddr);

#if defined(__unix__) && defined(_GNU_SOURCE)
		int sockid = ::accept4(
			m_socket,
			reinterpret_cast<::sockaddr*>(&addr),
			&len,
			SOCK_NONBLOCK);
#else
		int sockid = ::accept(m_socket, reinterpret_cast<::sockaddr*>(&addr), &len);
#endif
		if(sockid == -1)
			return false;

		if(socket)
			socket.close();

		socket.m_address = to_socket_address(
			reinterpret_cast<::sockaddr const&>(addr));

		socket.m_protocol = m_protocol;
		socket.m_type = m_type;
		socket.m_socket = sockid;

#if defined(__unix__) && defined(_GNU_SOURCE)
		socket.m_async = true;
#else
		socket.m_async = false;
		socket.set_async(true);
#endif

		return true;
	}

	bool Socket::pending(
		std::size_t ns_timeout)
	{
		assert(Runtime::exists());
		assert(exists());

		struct ::pollfd fd;
		fd.fd = m_socket;
		fd.events = POLLIN;

		::timespec ts;
		ts.tv_sec = ns_timeout / 1000000000;
		ts.tv_nsec = ns_timeout % 1000000000;

		::poll(&fd, 1, ns_timeout);
		return fd.revents & POLLIN;
	}

	void Socket::pending(
		Socket const * const * sockets,
		bool * pending,
		std::size_t count,
		std::size_t us_timeout)
	{
		assert(Runtime::exists());
		assert(sockets != nullptr);
		assert(pending != nullptr);

		fd_set set;
		FD_ZERO(&set);

		std::uintptr_t highest_socket = 0;
		for(std::size_t i = 0; i < count; i++)
		{
			FD_SET(sockets[i]->m_socket, &set);
			if(sockets[i]->m_socket > highest_socket)
				highest_socket = sockets[i]->m_socket;
		}

		timeval val = { long(us_timeout / 1000000), long(us_timeout % 1000000) };

		::select(highest_socket+1, &set, 0,0, &val);

		for(std::size_t i = 0; i < count; i++)
			pending[i] = FD_ISSET(sockets[i]->m_socket, &set);
	}

	bool StreamSocket::listen()
	{
		assert(Runtime::exists());
		assert(exists());

		return !::listen(m_socket, SOMAXCONN);
	}

	StreamSocket::StreamSocket(
		AddressFamily family):
		Socket(family, SocketType::kStream)
	{
		assert(family == AddressFamily::kIPv4
			|| family == AddressFamily::kIPv6);
	}

	StreamSocket::StreamSocket(
		async_t,
		AddressFamily family):
		Socket(kAsync, family, SocketType::kStream)
	{
		assert(family == AddressFamily::kIPv4
			|| family == AddressFamily::kIPv6);
	}

	Socket::Socket(
		Socket &&move):
		m_address(move.m_address),
		m_protocol(move.m_protocol),
		m_type(move.m_type),
		m_socket(move.m_socket),
		m_async(move.m_async)
	{
		assert(&move != this);

		move.m_socket = -1;
	}

	Socket &Socket::operator=(
		Socket &&move)
	{
		if(&move == this)
			return *this;

		close();
		m_socket = move.m_socket;
		m_type = move.m_type;
		m_protocol = move.m_protocol;
		m_address = move.m_address;
		m_async = move.m_async;
		move.m_socket = -1;

		return *this;
	}
}