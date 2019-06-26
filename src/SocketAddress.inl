#ifndef __netlib_socketaddress_inl_hpp_defined
#define __netlib_socketaddress_inl_hpp_defined

namespace netlib
{
	constexpr IPv4Address::IPv4Address(
		ubyte_t d0,
		ubyte_t d1,
		ubyte_t d2,
		ubyte_t d3):
		d0(d0),
		d1(d1),
		d2(d2),
		d3(d3)
	{
	}

	constexpr IPv6Address::IPv6Address(
		std::uint16_t d0,
		std::uint16_t d1,
		std::uint16_t d2,
		std::uint16_t d3,
		std::uint16_t d4,
		std::uint16_t d5,
		std::uint16_t d6,
		std::uint16_t d7):
		d0(d0),
		d1(d1),
		d2(d2),
		d3(d3),
		d4(d4),
		d5(d5),
		d6(d6),
		d7(d7)
	{
	}

	constexpr IPv4SocketAddress::IPv4SocketAddress(
		IPv4Address const& address,
		std::uint16_t port):
		address(address),
		port(port)
	{
	}

	constexpr bool IPv4Address::operator==(IPv4Address const& other) const
	{
		return reinterpret_cast<std::int32_t const&>(*this)
			== reinterpret_cast<std::int32_t const&>(other);
	}
	constexpr bool IPv4Address::operator!=(IPv4Address const& other) const
	{
		return reinterpret_cast<std::int32_t const&>(*this)
			!= reinterpret_cast<std::int32_t const&>(other);
	}
	constexpr bool IPv4Address::operator!() const
	{
		return !reinterpret_cast<std::int32_t const&>(*this);
	}

	constexpr bool IPv4SocketAddress::operator==(IPv4SocketAddress const& other) const
	{
		return port == other.port && address == other.address;
	}
	constexpr bool IPv4SocketAddress::operator!=(IPv4SocketAddress const& other) const
	{
		return port != other.port || address != other.address;
	}

	constexpr bool IPv6Address::operator==(IPv6Address const& other) const
	{
		return !operator!=(other);
	}

	constexpr bool IPv6Address::operator!=(IPv6Address const& other) const
	{
		return ((reinterpret_cast<std::uint64_t const *>(this)[0]
				^ reinterpret_cast<std::uint64_t const *>(&other)[0])
			| (reinterpret_cast<std::uint64_t const *>(this)[1]
				^ reinterpret_cast<std::uint64_t const *>(&other)[1]));
	}
}

#endif