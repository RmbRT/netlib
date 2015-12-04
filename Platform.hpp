/* Netlib - platform.hpp
	this file contains the platform specific includes for the socket headers.
*/

#ifndef __netlib_platform_hpp_defined
#define __netlib_platform_hpp_defined

#ifdef _WIN32
#define NETLIB_WINDOWS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <WS2tcpip.h>
#ifdef _MSC_VER
#pragma comment(lib,"ws2_32")
#endif
#else
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#endif


#endif