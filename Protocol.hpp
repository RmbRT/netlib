#ifndef __netlib_protocol_hpp_defined
#define __netlib_protocol_hpp_defined

namespace netlib
{
	/* IPPROTO_* constants. Some are left out as they were not deemed necessary. */
	enum class Protocol: int
	{
		/*Automatically chooses the default Protocol for a @{SocketType}*/
		Default = 0,
		/*TCP*/
		TransmissionControl = 6, TCP = TransmissionControl,
		/*UDP*/
		UserDatagram = 17, UDP = UserDatagram,
		Raw = 255
	};
}

#endif