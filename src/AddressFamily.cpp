#include "AddressFamily.hpp"
#include "internal/platform.hpp"
#include <cassert>
#include <cstddef>
#include <iostream>

namespace netlib
{
	int to_native_api(AddressFamily family)
	{
		static int const k_lookup[] = {
			AF_UNIX,
			AF_INET,
			AF_INET6,
			AF_IRDA,
			AF_BLUETOOTH
		};

		static_assert(
			_countof(k_lookup) == NETLIB_COUNT(AddressFamily),
			"invalid entry count of lookup table.");

		assert((std::size_t)family < NETLIB_COUNT(AddressFamily));

		return k_lookup[(unsigned)family];
	};

	AddressFamily to_address_family(
		int native)
	{
		switch(native)
		{
		case AF_UNIX: return AddressFamily::kUnix;	
		case AF_INET: return AddressFamily::kIPv4;
		case AF_INET6: return AddressFamily::kIPv6;
		case AF_IRDA: return AddressFamily::kIrDA;
		case AF_BLUETOOTH: return AddressFamily::kBluetooth;
		default:
			std::cerr << __FILE__ ", to_address_family: Unhandled address family: " << native << " (needs ntohl?)\naborting.\n";
			std::abort();
		}
	}
}