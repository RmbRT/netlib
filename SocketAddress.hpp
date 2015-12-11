#ifndef __netlib_socketaddress_hpp_defined
#define __netlib_socketaddress_hpp_defined

#include "Defines.hpp"
#include "AddressFamily.hpp"

namespace netlib
{
	typedef unsigned char ubyte_t;
	typedef std::uint16_t port_t;

	PACKED_STRUCT IPv4Address
	{
		IPv4Address(): d0(0), d1(0), d2(0), d3(0) { }
		IPv4Address(ubyte_t d0, ubyte_t d1, ubyte_t d2, ubyte_t d3): d0(d0), d1(d1), d2(d2), d3(d3) { }
		/*format: d0.d1.d2.d3*/
		IPv4Address(const char* data) { IPv4Address::parse(data, *this); }

		/*format: d0.d1.d2.d3*/
		static bool parse(const char* data, IPv4Address &out);
			
		static const IPv4Address Any;

		NETLIB_INL bool operator==(const IPv4Address &other) const { return reinterpret_cast<const int&>(*this) == reinterpret_cast<const int&>(other); }
		NETLIB_INL bool operator!=(const IPv4Address &other) const { return reinterpret_cast<const int&>(*this) != reinterpret_cast<const int&>(other); }
		NETLIB_INL bool operator!() const { return reinterpret_cast<const int&>(*this); }

		ubyte_t d0, d1, d2, d3;
	};

	PACKED_STRUCT IPv6Address
	{
		IPv6Address() : d0(0), d1(0), d2(0), d3(0), d4(0), d5(0), d6(0), d7(0) { }
		IPv6Address(std::uint16_t d0, std::uint16_t d1, std::uint16_t d2, std::uint16_t d3, std::uint16_t d4, std::uint16_t d5, std::uint16_t d6, std::uint16_t d7) : d0(d0), d1(d1), d2(d2), d3(d3), d4(d4), d5(d5), d6(d6), d7(d7) { }
		/*format: d0:d1:d2:d3:d4:d5:d6:d7*/
		IPv6Address(const char* data) : IPv6Address() { IPv6Address::parse(data, *this); }

		/*format: d0:d1:d2:d3:d4:d5:d6:d7*/
		static bool parse(const char* data, IPv6Address &out);

		std::uint16_t d0, d1, d2, d3, d4, d5, d6, d7;
	};

	PACKED_STRUCT IPv6SocketAddress
	{
		port_t port;
		std::uint32_t field;
		IPv6Address address;
		std::uint32_t scope;
	};

	PACKED_STRUCT IPv4SocketAddress
	{
		IPv4SocketAddress(const IPv4Address &address, std::uint16_t port): port(port), address(address) { }
		/*format: ipv4:port (%u.%u.%u.%u:%u)
		ex.: "127.0.0.1:80"*/
		IPv4SocketAddress(const char* data) { IPv4SocketAddress::parse(data, *this); }
		IPv4SocketAddress(): address(), port() { }

		static bool parse(const char* data, IPv4SocketAddress &out);

		bool operator==(const IPv4SocketAddress &other) const { return (address == other.address) && (port == other.port); }
		bool operator!=(const IPv4SocketAddress &other) const { return (address != other.address) || (port != other.port); }
			
		port_t port;
		IPv4Address address;
		ubyte_t unused[8];
	};

	PACKED_STRUCT SocketAddress
	{
	private:
		friend class SocketBase;
		SocketAddress(AddressFamily family): family(family), raw({{0x00}}) {}
	public:
		SocketAddress(const IPv4SocketAddress &address) : family(AddressFamily::IPv4), raw(reinterpret_cast<const Address&>(address)) {}
		SocketAddress(const char* str);
		SocketAddress(): family(), raw() { }
		
		static bool parse(const char *str, SocketAddress &out);

		AddressFamily family;
		struct Address{ ubyte_t data[14]; } raw;

		NETLIB_INL IPv4SocketAddress &IPv4() { return *reinterpret_cast<IPv4SocketAddress*>(raw.data); }
		NETLIB_INL const IPv4SocketAddress &IPv4() const { return *reinterpret_cast<const IPv4SocketAddress*>(raw.data); }
	};
}

#endif