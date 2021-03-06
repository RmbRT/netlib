/** @file SocketAddress.hpp
	Contains address structures, unifies them into the SocketAddress type. */
#ifndef __netlib_socketaddress_hpp_defined
#define __netlib_socketaddress_hpp_defined

#include "defines.hpp"
#include "AddressFamily.hpp"
#include <cstdint>

namespace netlib
{
	typedef std::uint8_t ubyte_t;
	typedef std::uint16_t port_t;

	/** Represents an IPv4 address. */
	struct alignas(4) IPv4Address
	{
		/** The fields of the ip address. */
		ubyte_t d0, d1, d2, d3;

		IPv4Address() = default;
		/** Constructs an IPv4 address. */
		constexpr IPv4Address(
			ubyte_t d0,
			ubyte_t d1,
			ubyte_t d2,
			ubyte_t d3);

		/** format: d0.d1.d2.d3*/
		IPv4Address(
			char const * data);

		/** format: d0.d1.d2.d3*/
		static bool parse(
			char const * data,
			IPv4Address &out);

		/** The 'any' ip address. */
		static IPv4Address const kAny;

		/** Tests for equality of two ip addresses. */
		constexpr bool operator==(IPv4Address const& other) const;
		/** Tests for unequality of two ip addresses. */
		constexpr bool operator!=(IPv4Address const& other) const;
		/** Tests for invalidity of an ip address. */
		constexpr bool operator!() const;
	};

	/** Represents an IPv6 address. */
	struct alignas(8) IPv6Address
	{
		/** The ip address data. */
		std::uint16_t d0, d1, d2, d3, d4, d5, d6, d7;

		IPv6Address() = default;
		/** Creates an IPv6 address. */
		constexpr IPv6Address(
			std::uint16_t d0,
			std::uint16_t d1,
			std::uint16_t d2,
			std::uint16_t d3,
			std::uint16_t d4,
			std::uint16_t d5,
			std::uint16_t d6,
			std::uint16_t d7);
		/** Parses an ipv6 address.
			Does not throw on failure, so it is not as safe as ```parse()```.
		@param[in] data:
			The address.
			Format: d0:d1:d2:d3:d4:d5:d6:d7 */
		IPv6Address(
			char const * data);

		/** Parses an ipv6 address.
		@param[in] data:
			The address.
			Format: d0:d1:d2:d3:d4:d5:d6:d7
		@param[out] out:
			The parsed result.
		@return
			Whether the string was a valid address. */
		static bool parse(
			char const * data,
			IPv6Address &out);

		/** Tests for equality of two IP addresses. */
		constexpr bool operator==(IPv6Address const& other) const;
		/** Tests for inequality of two IP addresses. */
		constexpr bool operator!=(IPv6Address const&) const;
	};

	/** IPv4 socket address. */
	struct IPv4SocketAddress
	{
		/** The address. */
		IPv4Address address;
		/** The port of the address. */
		port_t port;

		IPv4SocketAddress() = default;
		/** */
		constexpr IPv4SocketAddress(
			IPv4Address const& address,
			port_t port);
		/*format: ipv4:port (%u.%u.%u.%u:%u)
		ex.: "127.0.0.1:80"*/
		IPv4SocketAddress(char const * data);

		static bool parse(
			char const * data,
			IPv4SocketAddress &out);

		constexpr bool operator==(IPv4SocketAddress const& other) const;
		constexpr bool operator!=(IPv4SocketAddress const& other) const;
	};

	/** IPv6 socket address. */
	struct IPv6SocketAddress
	{
		IPv6Address address;
		std::uint32_t field;
		std::uint32_t scope;
		port_t port;

		bool operator==(IPv6SocketAddress const& other) const;
		bool operator!=(IPv6SocketAddress const& other) const;
	};


	/** Generic socket address. */
	struct SocketAddress
	{
		SocketAddress() = default;
		SocketAddress(AddressFamily family);
		SocketAddress(IPv4SocketAddress const& address);
		SocketAddress(IPv6SocketAddress const& address);
		SocketAddress(char const * str);

		static bool parse(
			char const * str,
			SocketAddress &out);

		bool operator==(SocketAddress const& other) const;
		bool operator!=(SocketAddress const& other) const;

		AddressFamily family;
		union
		{
			IPv4SocketAddress ipv4;
			IPv6SocketAddress ipv6;
		} address;
	};
}

#include "SocketAddress.inl"

#endif