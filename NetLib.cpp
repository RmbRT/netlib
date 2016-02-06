#include "NetLib.hpp"

#include "Platform.hpp"
#include <cassert>
#include <cstdio>


namespace netlib
{
	bool initialized = false;
	void cleanupSocketAPI()
	{
#ifdef NETLIB_WINDOWS
		if(initialized)
		{
			WSACleanup();
			initialized = false;
		}
#endif
	}

	bool startupSocketAPI()
	{
#ifdef NETLIB_WINDOWS
		if(!initialized)
		{
			WSADATA wsaData;
			return initialized = !WSAStartup(MAKEWORD(1,1), &wsaData);
		}
		else
			return true;
#else
		return initialized = true;
#endif
	}

	bool isSocketAPIInitialized()
	{
		return initialized;
	}




	
}