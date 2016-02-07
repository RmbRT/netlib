#include "SocketAddress.hpp"
#include <cstdio>
#include "NetLib.hpp"
#include "HostInfo.hpp"

namespace netlib
{
	IPv4Address const IPv4Address::Any = IPv4Address(0,0,0,0);

	IPv4Address::IPv4Address(ubyte_t d0, ubyte_t d1, ubyte_t d2, ubyte_t d3):
		d0(d0), d1(d1), d2(d2), d3(d3) { }
	IPv4Address::IPv4Address(char const * data)
	{
		parse(data, *this);
	}

	bool IPv4Address::parse(char const * data, IPv4Address &out)
	{
		unsigned u0, u1, u2, u3;
		if(std::sscanf(data, "%u.%u.%u.%u", &u0, &u1, &u2, &u3) == 4
			&& (((u0|u1|u2|u3) & ~0xff) == 0))
		{
			out = IPv4Address(u0, u1, u2, u3);
			return true;
		}
		else
			return false;
	}

	IPv6Address::IPv6Address(std::uint16_t d0, std::uint16_t d1, std::uint16_t d2, std::uint16_t d3, std::uint16_t d4, std::uint16_t d5, std::uint16_t d6, std::uint16_t d7):
		d0(d0), d1(d1), d2(d2), d3(d3), d4(d4), d5(d5), d6(d6), d7(d7) { }

	IPv6Address::IPv6Address(char const * data) { parse(data, *this); }

	bool IPv6Address::parse(char const * data, IPv6Address &out)
	{
		unsigned u0, u1, u2, u3, u4, u5, u6, u7;
		if((std::sscanf(data, "%x:%x:%x:%x:%x:%x:%x:%x",
			&u0, &u1, &u2, &u3, &u4, &u5, &u6, &u7) == 8) &&
			(((u0|u1|u2|u3|u4|u5|u6|u7) & ~0xffff) == 0))
		{
			out = IPv6Address(u0, u1, u2, u3, u4, u5, u6, u7);
			return true;
		}
		else
			return false;
	}

	IPv4SocketAddress::IPv4SocketAddress(IPv4Address const& address, std::uint16_t port):
		address(address), port(port) { }

	IPv4SocketAddress::IPv4SocketAddress(char const * addr)
	{
		parse(addr, *this);
	}

	bool IPv4SocketAddress::parse(char const * data, IPv4SocketAddress &out)
	{
		unsigned int d0, d1, d2, d3, port;
		if((sscanf_s(data, "%u.%u.%u.%u:%u", &d0, &d1, &d2, &d3, &port)==5)
			&&(((d0|d1|d2|d3) & ~0xff) == 0) && ((port & ~0xffff) == 0))
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
	
	SocketAddress::SocketAddress(AddressFamily family):
		family(family) { }
	SocketAddress::SocketAddress(IPv4SocketAddress const&ipv4):
		family(AddressFamily::IPv4)
	{
		address.ipv4 = ipv4;
	}
	SocketAddress::SocketAddress(const char* str)
	{
		parse(str, *this);
	}
	bool SocketAddress::parse(const char * str, SocketAddress &out)
	{
		SocketAddress sa;
		if(IPv4SocketAddress::parse(str, sa.address.ipv4))
		{
			out = sa.address.ipv4;
			return true;
		}
#ifndef NETLIB_IPV6_WIP
		else if(IPv6SocketAddress::parse(str, sa.address.ipv6))
		{
			out = sa.address.ipv6;
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
						out = IPv6SocketAddress(hi.addresses().front().address.ipv6.address, atoi(&str[last_colon+1]));
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