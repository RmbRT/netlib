#include "Runtime.hpp"
#include "internal/platform.hpp"
#include <exception>

#define NETLIB_WSA_VERSION MAKEWORD(1,1)


namespace netlib
{
	lock::ThreadSafe<std::size_t> Runtime::s_instances(0);

	Runtime::Runtime()
	{
		lock::WriteLock<std::size_t> w_instances(s_instances);
		if(!(*w_instances)++)
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
		lock::WriteLock<std::size_t> w_instances(s_instances);

		// the last instance?
		if(! --(*w_instances))
		{
			// if on windows, cleanup the windows socket api.
#ifdef NETLIB_WINDOWS
			WSACleanup();
#endif
		}
	}
}