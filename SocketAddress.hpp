#ifndef __netlib_socketaddress_hpp_defined
#define __netlib_socketaddress_hpp_defined

#include "Defines.hpp"
#include "AddressFamily.hpp"

namespace netlib
{
	typedef std::uint8_t ubyte_t;
	typedef std::uint16_t port_t;

	PACKED_STRUCT IPv4Address
	{
		ubyte_t d0, d1, d2, d3;

		IPv4Address() = default;
		IPv4Address(ubyte_t d0, ubyte_t d1, ubyte_t d2, ubyte_t d3);
		/*format: d0.d1.d2.d3*/
		IPv4Address(char const * data);

		/*format: d0.d1.d2.d3*/
		static bool parse(char const * data, IPv4Address &out);
			
		static IPv4Address const Any;

		NETLIB_INL bool operator==(IPv4Address const& other) const;
		NETLIB_INL bool operator!=(IPv4Address const& other) const;
		NETLIB_INL bool operator!() const;

	};

	PACKED_STRUCT IPv6Address
	{
		std::uint16_t d0, d1, d2, d3, d4, d5, d6, d7;

		IPv6Address() = default;
		IPv6Address(std::uint16_t d0, std::uint16_t d1, std::uint16_t d2, std::uint16_t d3, std::uint16_t d4, std::uint16_t d5, std::uint16_t d6, std::uint16_t d7);
		/*format: d0:d1:d2:d3:d4:d5:d6:d7*/
		IPv6Address(char const * data);

		/*format: d0:d1:d2:d3:d4:d5:d6:d7*/
		static bool parse(char const * data, IPv6Address &out);
	};

	PACKED_STRUCT IPv4SocketAddress
	{	
		port_t port;
		IPv4Address address;
		ubyte_t unused[8];

		IPv4SocketAddress() = default;
		IPv4SocketAddress(IPv4Address const& address, std::uint16_t port);
		/*format: ipv4:port (%u.%u.%u.%u:%u)
		ex.: "127.0.0.1:80"*/
		IPv4SocketAddress(char const * data);

		static bool parse(char const * data, IPv4SocketAddress &out);

		NETLIB_INL bool operator==(IPv4SocketAddress const& other) const;
		NETLIB_INL bool operator!=(IPv4SocketAddress const& other) const;
	};

	PACKED_STRUCT IPv6SocketAddress
	{
		port_t port;
		std::uint32_t field;
		IPv6Address address;
		std::uint32_t scope;
	};


	PACKED_STRUCT SocketAddress
	{
		SocketAddress() = default;
		SocketAddress(AddressFamily family);
		SocketAddress(IPv4SocketAddress const& address);
		SocketAddress(char const * str);
		
		static bool parse(char const * str, SocketAddress &out);

		AddressFamily family;
		union
		{
			/*Anonymous struct VS workaround...*/
			struct { IPv4SocketAddress ipv4; };
			struct { IPv6SocketAddress ipv6; };
		} address;
	};
}

#include "SocketAddress.inl"

#endif