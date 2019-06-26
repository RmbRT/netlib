#include "Socket.hpp"
#include "internal/platform.hpp"
#include "internal/SocketAddress.hpp"
#include "Runtime.hpp"

#include <cassert>
#include <type_traits>
#include <cstring>
#include <stdexcept>

#include <poll.h>
#include <fcntl.h>
#include <cerrno>

#ifndef SOCKET_ERROR
int const SOCKET_ERROR = -1;
#endif

namespace netlib
{
	static int to_native_api(
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

	static Status parse_errno()
	{
		switch(errno)
		{
		case EWOULDBLOCK:
#if EWOULDBLOCK != EAGAIN
		case EAGAIN:
#endif
			return Status::kNotReady;
		case EINPROGRESS:
			return Status::kInProgress;
		default:
			return Status::kError;
		}
	}

	Socket::Socket(
		SocketAddress const& address,
		SocketType type,
		Protocol protocol):
		Socket(
			address.family,
			type,
			protocol)
	{
		m_address = address;
	}

	Socket::Socket(
		AddressFamily family,
		SocketType type,
		Protocol protocol):
		m_address(family),
		m_type(type),
		m_protocol(protocol),
		m_socket()
	{
		assert(Runtime::exists());

// Create nonblocking socket atomically, if possible.
#ifdef __unix__
		m_socket = ::socket(
			to_native_api(family),
			to_native_api(type) | SOCK_NONBLOCK,
			to_native_api(protocol));
#else
		m_socket = ::socket(
			to_native_api(family),
			to_native_api(type)),
			to_native_api(protocol));
#endif
		if(!exists())
			throw std::runtime_error("Failed to create socket.");

#ifdef NETLIB_WINDOWS
		unsigned long mode = 1;
		if(0 != ioctlsocket(m_socket, FIONBIO, &mode))
			throw std::runtime_error("Failed to set socket to async.");
#endif
	}

	Socket::Socket():
		m_address(),
		m_type(),
		m_protocol(),
		m_socket(-1)
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
			m_input.fail_all();
			m_output.fail_all();

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

	bool Socket::bind(
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
		::sockaddr_storage addr;
		from_socket_address(m_address, addr);
		return SOCKET_ERROR != ::bind(
			m_socket,
			reinterpret_cast<::sockaddr const *>(&addr),
			sizeof(::sockaddr));
	}

	Status Socket::connect(
		SocketAddress const& serverAddress)
	{
		assert(Runtime::exists());
		assert(exists());

		m_address = serverAddress;
		::sockaddr_storage addr;
		from_socket_address(serverAddress, addr);

		if(SOCKET_ERROR == ::connect(
			m_socket,
			reinterpret_cast<::sockaddr const *>(&addr),
			sizeof(::sockaddr)))
			return parse_errno();
		else
			return Status::kSuccess;
	}

#ifndef NETLIB_WINDOWS
#define SD_RECEIVE SHUT_RD
#define SD_SEND SHUT_WR
#define SD_BOTH SHUT_RDWR
#endif

	Status Socket::shutdown(Shutdown param)
	{
		assert(Runtime::exists());
		assert(exists());

		static int const parameter[] = {
			SHUT_RD,
			SHUT_WR,
			SHUT_RDWR
		};
		assert(std::size_t(param) < _countof(parameter));

		if(!::shutdown(m_socket, parameter[std::size_t(param)]))
			return Status::kSuccess;
		else
			return parse_errno();
	}

	Status Socket::send(
		void const * data,
		size_t size,
		std::size_t &sent)
	{
		assert(Runtime::exists());
		assert(exists());

		std::size_t result = ::send(
			m_socket,
			(const char*)data,
			size,
			0);

		if(result == -1)
		{
			sent = 0;
			return parse_errno();
		} else
		{
			sent = result;
			return Status::kSuccess;
		}
	}

	Status Socket::recv(
		void * data,
		size_t size,
		std::size_t &received)
	{
		assert(Runtime::exists());
		assert(exists());

		std::size_t result = ::recv(
			m_socket,
			(char*)data,
			size,
			0);

		if(result == -1)
			return parse_errno();

		received = result;;
		return Status::kSuccess;
	}

	Status Socket::sendto(
		void const * data,
		size_t size,
		SocketAddress const& to,
		std::size_t &sent)
	{
		assert(Runtime::exists());
		assert(exists());

		::sockaddr_storage addr;
		from_socket_address(m_address, addr);
		std::size_t result = ::sendto(
			m_socket,
			(const char*)data,
			size,
			0,
			reinterpret_cast<::sockaddr const *>(&addr),
			sizeof(::sockaddr));

		if(result == -1)
			return parse_errno();

		sent = result;
		return Status::kSuccess;
	}

	Status Socket::recvfrom(
		void * data,
		std::size_t size,
		SocketAddress &from,
		std::size_t &received)
	{
		assert(Runtime::exists());
		assert(exists());

		::sockaddr_storage addr;
		::socklen_t len = sizeof(::sockaddr);

		std::size_t result = ::recvfrom(
			m_socket,
			(char*)data,
			size,
			0,
			reinterpret_cast<::sockaddr*>(&addr),
			&len);

		if(result == -1)
			return parse_errno();

		received = result;
		to_socket_address(reinterpret_cast<::sockaddr&>(addr), from);
		return Status::kSuccess;
	}

	Status StreamSocket::accept(
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
		int sockid = ::accept(
			m_socket,
			reinterpret_cast<::sockaddr*>(&addr),
			&len);
#endif
		if(sockid == -1)
			return parse_errno();

		socket.close();

		to_socket_address(
			reinterpret_cast<::sockaddr const&>(addr),
			socket.m_address);

		socket.m_protocol = m_protocol;
		socket.m_type = m_type;
		socket.m_socket = sockid;

#if !defined(__unix__) || !defined(_GNU_SOURCE)
		unsigned long mode = 1;
		if(0 != ioctlsocket(m_socket, FIONBIO, &mode))
			throw std::runtime_error("Failed to set socket to async.");
#endif
		return Status::kSuccess;
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

	Socket::Socket(
		Socket &&move):
		m_address(move.m_address),
		m_protocol(move.m_protocol),
		m_type(move.m_type),
		m_socket(move.m_socket)
	{
		assert(&move != this);

		assert(!move.m_input.fail_all());
		assert(!move.m_output.fail_all());
		move.m_socket = -1;
	}

	Socket &Socket::operator=(
		Socket &&move)
	{
		if(&move == this)
			return *this;

		assert(!m_input.fail_all());
		assert(!m_output.fail_all());
		close();
		m_socket = move.m_socket;
		m_type = move.m_type;
		m_protocol = move.m_protocol;
		m_address = move.m_address;
		move.m_socket = -1;

		assert(!move.m_input.fail_all());
		assert(!move.m_output.fail_all());

		return *this;
	}
}