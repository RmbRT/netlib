#include "HostInfo.hpp"
#include "internal/platform.hpp"
#include "internal/SocketAddress.hpp"
#include "Runtime.hpp"

#include <cassert>
#include <iostream>
#include <cstdlib>

namespace netlib
{
	std::vector<AddressInfo> resolve_name(
		char const * name)
	{
		assert(name != nullptr);
		assert(Runtime::exists());

		std::vector<AddressInfo> ret;

		::addrinfo * info;
		if(0 != getaddrinfo(
			name,
			nullptr,
			nullptr,
			&info))
		{
			return ret;
		}

		assert(info != nullptr);

		for(::addrinfo * it = info; it != nullptr; it = it->ai_next)
		{
			assert(it->ai_addr != nullptr);

			AddressInfo ai;
			to_socket_address(*it->ai_addr, ai.address);
			if(it->ai_canonname)
				ai.name = it->ai_canonname;
			ret.push_back(std::move(ai));
		};

		::freeaddrinfo(info);

		return std::move(ret);
	}
}