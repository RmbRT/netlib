#include "Protocol.hpp"
#include "internal/platform.hpp"

#include <cstddef>
#include <cassert>

namespace netlib
{
	int to_native_api(
		Protocol protocol)
	{
		static int const k_lookup[] = {
			0,
			IPPROTO_TCP,
			IPPROTO_UDP,
			IPPROTO_RAW
		};

		static_assert(
			_countof(k_lookup) == NETLIB_COUNT(Protocol),
			"Invalid lookup table size.");

		assert((std::size_t)protocol < NETLIB_COUNT(Protocol));

		return k_lookup[(std::size_t)protocol];
	}
}