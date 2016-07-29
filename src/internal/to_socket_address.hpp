#ifndef __netlib_internal_to_socket_address_hpp_defined
#define __netlib_internal_to_socket_address_hpp_defined


#include "../SocketAddress.hpp"
#include "platform.hpp"

namespace netlib
{
	/** Converts a native socket address to a socket address. */
	SocketAddress to_socket_address(
		::sockaddr const& addr);
}

#endif