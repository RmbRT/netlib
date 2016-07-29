/** @file platform.hpp
	Contains the platform specific includes for the socket headers.
	It is not to be included by applications using the library, only by the source files of the library that have to access the socket functions. */
#ifndef __netlib_platform_hpp_defined
#define __netlib_platform_hpp_defined

#ifdef _WIN32
/** @def NETLIB_WINDOWS Indicates that the target platform is microsoft's windows. */
#define NETLIB_WINDOWS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <WS2tcpip.h>
#ifdef _MSC_VER
// add the socket library
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



#define TO_NET_PORT htons
#define TO_HOST_PORT ntohs

#define TO_NET_FLOWINFO htonl
#define TO_HOST_FLOWINFO ntohl

#define TO_NET_IPV6DATA htons
#define TO_HOST_IPV6DATA ntohs

#define TO_NET_SCOPEID htonl
#define TO_HOST_SCOPEID ntohl

#endif