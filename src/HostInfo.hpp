/** @file HostInfo.hpp
	Contains the resolve_name function that resolves a host name to a list of addresses. */
#ifndef __netlib_hostinfo_hpp_defined
#define __netlib_hostinfo_hpp_defined

#include <string>
#include <vector>

#include "defines.hpp"
#include "AddressFamily.hpp"
#include "SocketAddress.hpp"

namespace netlib
{
	/** Address information used to store host information
		Describes an address that connects to the host described by a HostInfo. */
	struct AddressInfo
	{
		/** The canonical name of the address info. */
		std::string name;
		/** The address of the host. */
		SocketAddress address;
	};

	/** Resolves a host name.
	@param[in] name:
		The host's name.
	@return
		The addresses retrieved. Empty, if it could not be retrieved. */
	std::vector<AddressInfo> resolve_name(
		char const * name);
}

#endif