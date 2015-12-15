#ifndef __netlib_addressfamily_hpp_defined
#define __netlib_addressfamily_hpp_defined

#include "Defines.hpp"
#include <cinttypes>

namespace netlib
{
	/* Address family of a socket. */
	enum class AddressFamily : char
	{
		Unix = 1,
		Local = Unix,
		IPv4 = 2,
		IPX = 4,
		AppleTalk = 5,
		IPv6 = 10,
		IRDA = 23,
		Bluetooth = 31
	};
}

#endif