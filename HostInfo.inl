#ifndef __netlib_hostinfo_inl_defined
#define __netlib_hostinfo_inl_defined

namespace netlib
{
	std::string const& HostInfo::name() const { return m_name; }
	std::vector<std::string> const& HostInfo::aliases() const { return m_aliases; }
	AddressFamily HostInfo::address_type() const { return m_address_type; }
	std::vector<SocketAddress> const& HostInfo::addresses() const { return m_addresses; }
	bool HostInfo::valid() const { return m_valid; }
}

#endif