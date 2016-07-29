/** @file AddressFamily.hpp
	Contains the AddressFamily enumeration. */
#ifndef __netlib_addressfamily_hpp_defined
#define __netlib_addressfamily_hpp_defined

#include "defines.hpp"

namespace netlib
{
	/** Address family of a socket. */
	enum class AddressFamily
	{
		/** Machine local. */
		kUnix, kLocal = kUnix,
		/** Internet protocol version 4. */
		kIPv4,
		/** Internet protocol version 6. */
		kIPv6,
		/** Infrared. */
		kIrDA,
		/** Bluetooth. */
		NETLIB_LAST(kBluetooth)
	};

#ifdef NETLIB_BUILD
	/** Converts an address family to its corresponding native api constant. */
	int to_native_api(AddressFamily family);

	/** Converts a native api constant into an address family. */
	AddressFamily to_address_family(
		int native);
#endif
}

#endif