/* Netlib - netlib.hpp
	this file contains the socket classes that wrap around the platform specific socket libraries.*/

#ifndef __netlib_hpp_defined
#define __netlib_hpp_defined

#include <cstdint>
#include <cstddef>
#include <string>
#include <vector>
#include <cassert>

#include "Defines.hpp"

/*Indicates that IPv6 support was not completed yet.*/
#define NETLIB_IPV6_WIP

namespace netlib
{
	bool init();
	bool isInited();
	void cleanup();
}

#endif