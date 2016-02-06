#include "SocketAddress.hpp"
#include <cstdio>

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
	bool IPv4SocketAddress::parse(const char* data, IPv4SocketAddress &out)
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
}