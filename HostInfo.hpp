#ifndef __netlib_hostinfo_hpp_defined
#define __netlib_hostinfo_hpp_defined

#include <string>
#include <vector>

#include "Defines.hpp"
#include "AddressFamily.hpp"
#include "SocketAddress.hpp"

namespace netlib
{
	/* Stores the host information.
	If the host information could not be retrieved, valid() returns false. */
	class HostInfo
	{
		std::string m_name;
		std::vector<std::string> m_aliases;
		AddressFamily m_address_type;
		std::vector<SocketAddress> m_addresses;
		bool m_valid;
	public:
		HostInfo();
		HostInfo(HostInfo && move);
		HostInfo & operator=(HostInfo && move);
		
		/* Retrieves the host information of the given name. */
		explicit HostInfo(std::string name);

		NETLIB_INL const std::string &name() const;
		NETLIB_INL const std::vector<std::string> &aliases() const;
		NETLIB_INL AddressFamily address_type() const;
		NETLIB_INL const std::vector<SocketAddress> &addresses() const;
		NETLIB_INL bool valid() const;
	};
}

#endif