#include "to_socket_address.hpp"
#include <iostream>
#include <cstdlib>

namespace netlib
{


	SocketAddress to_socket_address(
		::sockaddr const& addr)
	{


		SocketAddress ret;
		ret.family = to_address_family(addr.sa_family);
		switch(ret.family)
		{
		case AddressFamily::kIPv4:
			{
				::sockaddr_in const& in4 =
					reinterpret_cast<::sockaddr_in const&>(
						addr);
				ret.address.ipv4.port = TO_HOST_PORT(in4.sin_port);
				ret.address.ipv4.address =
					reinterpret_cast<IPv4Address const&>(
						in4.sin_addr.s_addr);
			} break;
		case AddressFamily::kIPv6:
			{
				::sockaddr_in6 const& in6 =
					reinterpret_cast<::sockaddr_in6 const&>(
						addr);
				ret.address.ipv6.port = TO_HOST_PORT(in6.sin6_port);
				
				std::uint16_t const * const data = reinterpret_cast<std::uint16_t const *>(
					&in6.sin6_addr.s6_addr[0]);

				for(size_t i = 8; i--;)
					(&ret.address.ipv6.address.d0)[i] = TO_HOST_IPV6DATA(data[i]);
			} break;
		default:
			{
				std::cerr << __FILE__ ", to_socket_address: Address family not supported: " << (int) ret.family << "\naborting.\n";
				std::abort();
			} break;
		};

		return ret;
	}
}