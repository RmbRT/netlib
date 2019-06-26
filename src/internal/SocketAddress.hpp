#ifndef __netlib_internal_to_socket_address_hpp_defined
#define __netlib_internal_to_socket_address_hpp_defined

#include "../SocketAddress.hpp"
#include "platform.hpp"

namespace netlib
{
	/** Converts a native socket address to a socket address. */
	void to_socket_address(
		::sockaddr const& addr,
		SocketAddress &out);

	/** Converts a socket address to a native socket address. */
	void from_socket_address(
		SocketAddress const& addr,
		::sockaddr_storage &out);
}

#endif