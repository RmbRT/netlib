#include "NetLib.hpp"

#include "Platform.hpp"
#include <cassert>
#include <cstdio>


namespace netlib
{
	const IPv4Address IPv4Address::Any(0,0,0,0);

	bool initialized = false;
	void cleanupSocketAPI()
	{
#ifdef NETLIB_WINDOWS
		if(initialized)
		{
			WSACleanup();
			initialized = false;
		}
#endif
	}

	bool startupSocketAPI()
	{
#ifdef NETLIB_WINDOWS
		if(!initialized)
		{
			WSADATA wsaData;
			return initialized = !WSAStartup(MAKEWORD(1,1), &wsaData);
		}
		else
			return true;
#else
		return initialized = true;
#endif
	}

	bool isSocketAPIInitialized()
	{
		return initialized;
	}

	bool IPv4Address::parse(const char* data, IPv4Address &out)
	{
		unsigned int d0, d1, d2, d3;
		if((sscanf_s(data, "%u.%u.%u.%u", &d0, &d1, &d2, &d3)==4)
			&&(d0<=0xff)&&(d1<=0xff)&&(d2<=0xff)&&(d3<=0xff))
		{
			out.d0 = ubyte_t(d0);
			out.d1 = ubyte_t(d1);
			out.d2 = ubyte_t(d2);
			out.d3 = ubyte_t(d3);
			return true;
		}
		else
			return false;
	}

	bool IPv4SocketAddress::parse(const char* data, IPv4SocketAddress &out)
	{
		unsigned int d0, d1, d2, d3, port;
		if((sscanf_s(data, "%u.%u.%u.%u:%u", &d0, &d1, &d2, &d3, &port)==5)
			&&(d0<=0xff)&&(d1<=0xff)&&(d2<=0xff)&&(d3<=0xff)&&(port<=0xffff))
		{
			out.address.d0 = ubyte_t(d0);
			out.address.d1 = ubyte_t(d1);
			out.address.d2 = ubyte_t(d2);
			out.address.d3 = ubyte_t(d3);
			out.port = std::uint16_t(port);
			return true;
		}
		else
			return false;
	}

	::sockaddr_in _sockaddr_in(const SocketAddress &addr)
	{
		sockaddr_in saddr;
		memset(&saddr, 0, sizeof(saddr));
		saddr.sin_family = (short)addr.family;
		saddr.sin_addr.S_un.S_addr = reinterpret_cast<const u_long&>(addr.IPv4().address);
		saddr.sin_port = htons(addr.IPv4().port);
		return saddr;
	}

	SocketBase::SocketBase(const SocketAddress &address, SocketType type, Protocol protocol): address(address), type(type), protocol(protocol), socket(-1)
	{
		socket = ::socket((int)address.family, (int)type, (int)protocol);
	}
	SocketBase::SocketBase(AddressFamily family, SocketType type, Protocol protocol) : address(family), type(type), protocol(protocol), socket(-1)
	{
		socket = ::socket((int)address.family, (int)type, (int)protocol);
	}
	SocketBase::SocketBase() : address(IPv4SocketAddress("")), type(), protocol(), socket(-1) {}

	void SocketBase::close()
	{
		if(isOpen())
#ifdef NETLIB_WINDOWS
			::closesocket
#else
			::close
#endif
			(socket);

		socket = -1;
	}

	int SocketBase::shutdown(Shutdown param)
	{
		switch(param)
		{
		default:
			{
				assert(false);
				return -1;
			} break;
		case Shutdown::Receive:
			{
				return ::shutdown(socket, SD_RECEIVE);
			} break;
		case Shutdown::Send:
			{
				return ::shutdown(socket, SD_SEND);
			} break;
		case Shutdown::Both:
			{
				return ::shutdown(socket, SD_BOTH);
			} break;
		}
	}

	int SocketBase::send(const void* data, std::size_t size)
	{
		assert(protocol == Protocol::TCP || (protocol == Protocol::Default && type == SocketType::Stream));
		return ::send(socket, (const char*)data, size, 0);
	}
	int SocketBase::sendto(const SocketAddress &address, const void *data, std::size_t size)
	{
		::sockaddr_in addr = _sockaddr_in(address);
		assert(protocol == Protocol::UDP || (protocol == Protocol::Default && type == SocketType::Datagram));
		return ::sendto(socket, (const char*)data, size, 0, reinterpret_cast<const ::sockaddr*>(&addr), sizeof(::sockaddr));
	}
	int SocketBase::recv(void* data, std::size_t size)
	{
		assert(protocol == Protocol::TCP || (protocol == Protocol::Default && type == SocketType::Stream));
		return ::recv(socket, (char*)data, size, 0);
	}
	int SocketBase::recvfrom(const SocketAddress &address, void* data, std::size_t size)
	{
		assert(protocol == Protocol::UDP || (protocol == Protocol::Default && type == SocketType::Datagram));

		::sockaddr_in addr = _sockaddr_in(address);
		int len = sizeof(::sockaddr);
		return ::recvfrom(socket, (char*)data, size, 0, reinterpret_cast<::sockaddr*>(&addr), &len);
	}
	bool SocketBase::connect(const SocketAddress &serverAddress)
	{
		address = serverAddress;
		sockaddr_in addr = _sockaddr_in(serverAddress);
		return SOCKET_ERROR != ::connect(socket, reinterpret_cast<const ::sockaddr*>(&addr), sizeof(::sockaddr));
	}

	bool SocketBase::bind(const SocketAddress &address)
	{
		this->address = address;
		::sockaddr_in addr = _sockaddr_in(address);
		return SOCKET_ERROR != ::bind(socket, reinterpret_cast<const ::sockaddr*>(&addr), sizeof(::sockaddr));
	}
	bool SocketBase::accept(SocketBase &socket)
	{
		sockaddr_in addr;
		int len = sizeof(::sockaddr);
		int sockid = ::accept(this->socket, reinterpret_cast<::sockaddr*>(&addr), &len);
			
		if(sockid == -1)
			return false;

		if(socket)
			socket.close();

		socket.address = reinterpret_cast<SocketAddress&>(addr);
		socket.protocol = protocol;
		socket.type = type;
		socket.socket = sockid;

		return true;
	}
	
	bool SocketBase::pending()
	{
		fd_set set;
		FD_ZERO(&set);
		FD_SET(socket, &set);

		timeval val = { 0, 0 };
		
		::select(socket+1,&set,0,0,&val);
		return FD_ISSET(socket, &set);
	}

	bool SocketBase::listen()
	{
		return !::listen(socket, SOMAXCONN);
	}
	StreamSocket::StreamSocket(AddressFamily family) : SocketBase(family, SocketType::Stream)
	{
		assert(family == AddressFamily::IPv4 || family == AddressFamily::IPv6);
	}

	SocketBase::SocketBase(SocketBase &&move) : address(move.address), protocol(move.protocol), type(move.type), socket(move.socket)
	{
		assert(&move != this);
		move.socket = -1;
	}
	SocketBase &SocketBase::operator=(SocketBase &&move)
	{
		if(&move == this)
			return *this;

		close();
		socket = move.socket;
		type = move.type;
		protocol = move.protocol;
		address = move.address;
		move.socket = -1;

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

		

			HostInfo hi;
			if(HostInfo::hostInfoByName(_str.substr(0, last_colon), hi))
			{
				switch(hi.address_type())
				{
				case AddressFamily::IPv4:
					{
						out = IPv4SocketAddress(hi.ipv4_addresses().front(), atoi(&str[last_colon+1]));
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

	bool HostInfo::hostInfoByName(const std::string &name, HostInfo &out)
	{
		::sockaddr_in6 a;
		HostInfo info;
		hostent *hostent = gethostbyname(name.c_str());
		if(!hostent)
			return false;

		out._name = hostent->h_name;
		out._address_type = (AddressFamily)hostent->h_addrtype;

		unsigned i = 0;
		while(hostent->h_addr_list[i])
			i++;

		out._address_list_v4.clear();
		out._address_list_v6.clear();
		switch(out._address_type)
		{
		case AddressFamily::IPv4:
			while(i--)
			{
				out._address_list_v4.reserve(i);
				u_long temp = *reinterpret_cast<const u_long*>(hostent->h_addr_list[i]);
				//temp = ntohl(temp);
				const_cast<std::vector<IPv4Address>&>(out.ipv4_addresses()).push_back(reinterpret_cast<IPv4Address&>(temp));
			} break;
		default:
			{
				assert(false && "ip v6 addresses are not supported yet.");
			} break;
		}
		i = 0;
		while(hostent->h_aliases[i])
			i++;
		out._aliases.clear();
		out._aliases.reserve(i);
		while(i--)
			out._aliases.push_back(hostent->h_aliases[i]);
			
		return true;
	}
}