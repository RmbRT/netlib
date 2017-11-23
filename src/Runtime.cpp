#include "Runtime.hpp"
#include "internal/platform.hpp"
#include <exception>

#define NETLIB_WSA_VERSION MAKEWORD(1,1)


namespace netlib
{
	std::atomic_size_t Runtime::s_instances(0);

	Runtime::Runtime()
	{
		if(1 == s_instances.fetch_add(1, std::memory_order_relaxed))
		{
			// if on windows, attempt to initialise the windows socket api.
#ifdef NETLIB_WINDOWS
			WSADATA wsaData;
			if(0 != WSAStartup(NETLIB_WSA_VERSION, &wsaData))
			{	// failed to initialize?
				// throw exception.
				throw std::exception("could not initialise windows socket api.");
			}
#endif
		}
	}

	Runtime::~Runtime()
	{
		// the last instance?
		if(0 == s_instances.fetch_sub(1, std::memory_order_relaxed))
		{
			// if on windows, cleanup the windows socket api.
#ifdef NETLIB_WINDOWS
			WSACleanup();
#endif
		}
	}
}