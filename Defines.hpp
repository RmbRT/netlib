#ifndef __netlib_defines_hpp_defined
#define __netlib_defines_hpp_defined

#ifdef __GNUC__
#define PACKED_STRUCT struct __attribute__((__packed__))
#define PACKED_CLASS class __attribute__((__packed__))
#define PACKED_UNION union __attribute__((__packed__))
#else // should work for msvc, not sure about other compilers.
#define PACKED_STRUCT __declspec(align(1)) struct
#define PACKED_CLASS __declspec(align(1)) class
#define PACKED_UNION __declspec(align(1)) union
#endif

#ifdef _MSC_VER
#define NETLIB_INL __forceinline
#else
#define NETLIB_INL inline
#endif


#endif