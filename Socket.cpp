#include "Socket.hpp"
#include "Platform.hpp"
#include "NetLib.hpp"
#include "HostInfo.hpp"
#include <cassert>

namespace netlib
{
	
	::sockaddr_in _sockaddr_in(const SocketAddress &addr)
	{
		sockaddr_in saddr;
		memset(&saddr, 0, sizeof(saddr));
		saddr.sin_family = (short)addr.family;
		saddr.sin_addr.S_un.S_addr = reinterpret_cast<const u_long&>(addr.address.ipv4.address);
		saddr.sin_port = htons(addr.address.ipv4.port);
		return saddr;
	}


	Socket::Socket(const SocketAddress &address, SocketType type, Protocol protocol): m_address(address), m_type(type), m_protocol(protocol), m_socket(-1)
	{
		m_socket = ::socket((int)address.family, (int)type, (int)protocol);
	}
	Socket::Socket(AddressFamily family, SocketType type, Protocol protocol) : m_address(family), m_type(type), m_protocol(protocol), m_socket(-1)
	{
		m_socket = ::socket((int)m_address.family, (int)type, (int)protocol);
	}
	Socket::Socket() : m_address(IPv4SocketAddress("")), m_type(), m_protocol(), m_socket(-1) {}

	void Socket::close()
	{
		if(exists())
#ifdef NETLIB_WINDOWS
			::closesocket
#else
			::close
#endif
			(m_socket);

		m_socket = -1;
	}

	int Socket::shutdown(Shutdown param)
	{
		switch(param)
		{
		case Shutdown::Receive:
			{
				return ::shutdown(m_socket, SD_RECEIVE);
			} break;
		case Shutdown::Send:
			{
				return ::shutdown(m_socket, SD_SEND);
			} break;
		case Shutdown::Both:
			{
				return ::shutdown(m_socket, SD_BOTH);
			} break;
		default:
			{
				assert(false);
				return -1;
			} break;
		}
	}

	size_t StreamSocket::send(const void* data, size_t size)
	{
		assert(m_protocol == Protocol::TCP || (m_protocol == Protocol::Default && m_type == SocketType::Stream));
		return ::send(m_socket, (const char*)data, size, 0);
	}
	size_t DatagramSocket::sendto(void const * data, size_t size, SocketAddress const& to, size_t flags)
	{
		::sockaddr_in addr = _sockaddr_in(m_address);
		assert(m_protocol == Protocol::UDP || (m_protocol == Protocol::Default && m_type == SocketType::Datagram));
		return ::sendto(m_socket, (const char*)data, size, 0, reinterpret_cast<const ::sockaddr*>(&addr), sizeof(::sockaddr));
	}
	size_t StreamSocket::recv(void* data, size_t size)
	{
		assert(m_protocol == Protocol::TCP || (m_protocol == Protocol::Default && m_type == SocketType::Stream));
		return ::recv(m_socket, (char*)data, size, 0);
	}
	size_t DatagramSocket::recvfrom(void * data, size_t size, SocketAddress const& from, size_t flags)
	{
		assert(m_protocol == Protocol::UDP || (m_protocol == Protocol::Default && m_type == SocketType::Datagram));

		::sockaddr_in addr = _sockaddr_in(m_address);
		int len = sizeof(::sockaddr);
		return ::recvfrom(m_socket, (char*)data, size, 0, reinterpret_cast<::sockaddr*>(&addr), &len);
	}
	bool StreamSocket::connect(const SocketAddress &serverAddress)
	{
		m_address = serverAddress;
		sockaddr_in addr = _sockaddr_in(serverAddress);
		return SOCKET_ERROR != ::connect(m_socket, reinterpret_cast<const ::sockaddr*>(&addr), sizeof(::sockaddr));
	}

	bool StreamSocket::bind(const SocketAddress &address)
	{
		m_address = address;
		::sockaddr_in addr = _sockaddr_in(address);
		return SOCKET_ERROR != ::bind(m_socket, reinterpret_cast<const ::sockaddr*>(&addr), sizeof(::sockaddr));
	}
	bool StreamSocket::accept(StreamSocket &socket)
	{
		sockaddr_in addr;
		int len = sizeof(::sockaddr);
		int sockid = ::accept(m_socket, reinterpret_cast<::sockaddr*>(&addr), &len);
			
		if(sockid == -1)
			return false;

		if(m_socket)
			socket.close();

		socket.m_address = reinterpret_cast<SocketAddress&>(addr);
		socket.m_protocol = m_protocol;
		socket.m_type = m_type;
		socket.m_socket = sockid;

		return true;
	}
	
	bool Socket::pending()
	{
		fd_set set;
		FD_ZERO(&set);
		FD_SET(m_socket, &set);

		timeval val = { 0, 0 };
		
		::select(m_socket+1,&set,0,0,&val);
		return FD_ISSET(m_socket, &set);
	}

	bool StreamSocket::listen()
	{
		return !::listen(m_socket, SOMAXCONN);
	}
	StreamSocket::StreamSocket(AddressFamily family) : Socket(family, SocketType::Stream)
	{
		assert(family == AddressFamily::IPv4 || family == AddressFamily::IPv6);
	}

	Socket::Socket(Socket &&move) : m_address(move.m_address), m_protocol(move.m_protocol), m_type(move.m_type), m_socket(move.m_socket)
	{
		assert(&move != this);
		move.m_socket = -1;
	}
	Socket &Socket::operator=(Socket &&move)
	{
		if(&move == this)
			return *this;

		close();
		m_socket = move.m_socket;
		m_type = move.m_type;
		m_protocol = move.m_protocol;
		m_address = move.m_address;
		move.m_socket = -1;

		return *this;
	}

	SocketAddress::SocketAddress(const char* str)
	{
		parse(str, *this);
	}
	bool SocketAddress::parse(const char * str, SocketAddress &out)
	{
		
		IPv4SocketAddress ipv4sa;
		IPv6SocketAddress ipv6sa;
		if(IPv4SocketAddress::parse(str, ipv4sa))
		{
			out = ipv4sa;
			return true;
		}
#ifndef NETLIB_IPV6_WIP
		else if(IPv6SocketAddress::parse(str, ipv6sa))
		{
			out = ipv6sa;
			return true;
		}
#endif
		else
		{
			std::string _str(str);
			unsigned l = _str.length();
		
			unsigned last_colon;
			for(last_colon = l; last_colon--;)
				if(str[last_colon] == ':')
					break;

			if(!last_colon || last_colon == l-1)
				return false;

		

			HostInfo hi(_str.substr(0, last_colon));

			if(hi.valid())
			{
				switch(hi.address_type())
				{
				case AddressFamily::IPv4:
					{
						out = IPv4SocketAddress(hi.addresses().front().address.ipv4.address, atoi(&str[last_colon+1]));
						return true;
					} break;
#ifndef NETLIB_IPV6_WIP
				case AddressFamily::IPv6:
					{
						out = IPv6SocketAddress(hi.ipv6_addresses().front(), atoi(&str[last_colon+1]));
						return true;
					} break;
#endif
				default:
					{
						assert(!"invalid address family.");
						return false;
					} break;
				}
			}
			else
				return false;
		}
	}
}