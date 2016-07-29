/** @file Protocol.hpp
	Contains the Protocol enumeration. */
#ifndef __netlib_protocol_hpp_defined
#define __netlib_protocol_hpp_defined

#include "defines.hpp"

namespace netlib
{
	/** Socket protocol constants.
		Some are left out as they were deemed unnecessary. */
	enum class Protocol
	{
		/** Automatically chooses the default Protocol for a SocketType. */
		kDefault,
		/** TCP. */
		kTransmissionControl, kTCP = kTransmissionControl,
		/** UDP. */
		kUserDatagram, kUDP = kUserDatagram,
		/** Raw sockets. */
		NETLIB_LAST(kRaw)
	};

#ifdef NETLIB_BUILD
	/** Converts a protocol constant to the native socket api constant.
	@param[in] protocol:
		The enum constant to convert. */
	int to_native_api(Protocol protocol);
#endif
}

#endif