#include "SocketAddress.hpp"
#include <cstdio>
#include "HostInfo.hpp"
#include "netlib.hpp"

#include <cassert>

namespace netlib
{
	IPv4Address const IPv4Address::kAny = IPv4Address(0,0,0,0);


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

	IPv4SocketAddress::IPv4SocketAddress(char const * addr)
	{
		parse(addr, *this);
	}

	bool IPv4SocketAddress::parse(char const * data, IPv4SocketAddress &out)
	{
		unsigned int d0, d1, d2, d3, port;
		if((sscanf(data, "%u.%u.%u.%u:%u", &d0, &d1, &d2, &d3, &port)==5)
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

	bool IPv6SocketAddress::operator==(IPv6SocketAddress const& other) const
	{
		return port == other.port
			&& field == other.field
			&& address == other.address
			&& scope == other.scope;
	}

	bool IPv6SocketAddress::operator!=(IPv6SocketAddress const& other) const
	{
		return port != other.port
			|| field != other.field
			|| address != other.address
			|| scope != other.scope;
	}

	SocketAddress::SocketAddress(AddressFamily family):
		family(family) { }
	SocketAddress::SocketAddress(IPv4SocketAddress const& ipv4):
		family(AddressFamily::kIPv4)
	{
		address.ipv4 = ipv4;
	}
	SocketAddress::SocketAddress(IPv6SocketAddress const& ipv6):
		family(AddressFamily::kIPv6)
	{
		address.ipv6 = ipv6;
	}
	SocketAddress::SocketAddress(const char * str)
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

			port_t port = atoi(&str[last_colon+1]);
			if(port & ~0xffff != 0)
				return false;

			_str[last_colon] = '\0';

			std::vector<AddressInfo> info = resolve_name(_str.c_str());

			if(!info.empty())
			{
				out = info.front().address;
				switch(out.family)
				{
				case AddressFamily::kIPv4:
					{
						out.address.ipv4.port = port;
					} break;
				case AddressFamily::kIPv6:
					{
						out.address.ipv6.port = port;
					} break;
				default:
					{
						assert(!"Address family not supported.");
					} break;
				}
			}
			else
				return false;
		}
	}


	bool SocketAddress::operator==(SocketAddress const& other) const
	{
		if(family != other.family)
			return false;
		else if(family == AddressFamily::kIPv4)
		{
			return address.ipv4 == other.address.ipv4;
		} else if(family == AddressFamily::kIPv6)
		{
			return address.ipv6 == other.address.ipv6;
		} else
		{
			assert(!"Address family not supported.");
		}
	}

	bool SocketAddress::operator!=(SocketAddress const& other) const
	{
		return !(*this == other);
	}

}