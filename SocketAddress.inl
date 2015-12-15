#ifndef __netlib_socketaddress_inl_hpp_defined
#define __netlib_socketaddress_inl_hpp_defined

namespace netlib
{
	NETLIB_INL bool IPv4Address::operator==(IPv4Address const& other) const
	{
		return reinterpret_cast<std::int32_t const&>(*this)
			== reinterpret_cast<std::int32_t const&>(other);
	}
	NETLIB_INL bool IPv4Address::operator!=(IPv4Address const& other) const
	{
		return reinterpret_cast<std::int32_t const&>(*this)
			!= reinterpret_cast<std::int32_t const&>(other);
	}
	NETLIB_INL bool IPv4Address::operator!() const
	{
		return !reinterpret_cast<std::int32_t const&>(*this);
	}

	NETLIB_INL bool IPv4SocketAddress::operator==(IPv4SocketAddress const& other) const
	{
		return port == other.port && address == other.address;
	}
	NETLIB_INL bool IPv4SocketAddress::operator!=(IPv4SocketAddress const& other) const
	{
		return port != other.port || address != other.address;
	}
}

#endif