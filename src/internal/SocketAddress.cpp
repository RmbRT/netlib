#include "SocketAddress.hpp"
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cassert>

namespace netlib
{
	void to_socket_address(
		::sockaddr const& addr,
		SocketAddress &ret)
	{
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
				std::cerr << "to_socket_address: Address family not supported: " << (int) ret.family << "\naborting.\n";
				std::abort();
			} break;
		};
	}

	void from_socket_address(
		SocketAddress const& addr,
		::sockaddr_storage &ret)
	{
		memset(&ret, 0, sizeof(ret));

		ret.ss_family = to_native_api(addr.family);

		switch(addr.family)
		{
		case AddressFamily::kIPv4:
			{
				::sockaddr_in & sin = reinterpret_cast<::sockaddr_in &>(ret);
				IPv4SocketAddress const& ipv4 = addr.address.ipv4;

				// set fields.
				sin.sin_port = TO_NET_PORT(ipv4.port);
				// no need to cast, since the byte array is in the right order already.
				sin.sin_addr.s_addr = reinterpret_cast<std::uint32_t const&>(ipv4.address);
			} break;
		case AddressFamily::kIPv6:
			{
				::sockaddr_in6 & sin6 = reinterpret_cast<::sockaddr_in6 &>(ret);
				IPv6SocketAddress const& ipv6 = addr.address.ipv6;

				// set fields.
				sin6.sin6_port = TO_NET_PORT(ipv6.port);
				sin6.sin6_flowinfo = TO_NET_FLOWINFO(ipv6.field);

				std::uint16_t * const words = reinterpret_cast<std::uint16_t *>(
					&sin6.sin6_addr.s6_addr[0]);

				// convert 2-Byte pairs to the right byte order.
				for(std::size_t i = 8; i--;)
					words[i] = TO_NET_IPV6DATA((&ipv6.address.d0)[i]);

				sin6.sin6_scope_id = TO_NET_SCOPEID(ipv6.scope);
			} break;
		default:
			{
				assert(!"invalid address family");
			} break;
		};
	}
}