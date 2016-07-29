/** @file defines.hpp
	Contains defines used for different*/
#ifndef __netlib_defines_hpp_defined
#define __netlib_defines_hpp_defined

/** @def NETLIB_PACKED(tag)
	Creates a packed type.
	Example: NETLIB_PACKED(class) creates a packed class. */
#ifdef __GNUC__
#define NETLIB_PACKED(tag) tag __attribute__((__packed__))
#elif defined(_MSC_VER)
#define NETLIB_PACKED(tag) __declspec(align(1)) tag
#else
#error "Compiler not supported"
#endif

/** @def NETLIB_INL
	Makes a function inline, and in case of MSVC, makes it __forceinline. */
#ifdef _MSC_VER
#define NETLIB_INL __forceinline
#else
#define NETLIB_INL inline
#endif


#define NETLIB_LAST(x) x, k_netlib_last_enum_value = x
#define NETLIB_COUNT(Enum) (static_cast<unsigned>(Enum::k_netlib_last_enum_value)+1)

#ifndef _countof
#define _countof(x) (sizeof((x)) / sizeof(*(x)))
#endif

#endif