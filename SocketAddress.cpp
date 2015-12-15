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
			&& ((u0|u1|u2|u3) & ~0xff == 0))
		{
			out =IPv4Address(d0,d1,d2,d3);
			return true;
		}
		else
			return false;
	}
}