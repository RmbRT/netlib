/** @file Runtime.hpp
	Contains the Runtime class that handles the socket API's initialising. */
#ifndef __netlib_runtime_hpp_defined
#define __netlib_runtime_hpp_defined

#include <cstddef>
#include <Lock/Lock.hpp>
#include "defines.hpp"

namespace netlib
{
	/** Instantiate an object of this type in every thread that has to use the netlib.
		Upon construction, initialises the socket api used by the target platform in case it was not initialised yet, and if it fails, throws an exception. Thus, it is guaranteed that as long as a runtime object exists, the socket api will be initialised. Upon destruction, cleans up the socket api, if the destroyed instance was the last one existing. */
	class Runtime
	{
		/** The number of currently existing instances. */
		static lock::ThreadSafe<std::size_t> s_instances;

		Runtime(Runtime const&) = delete;
		Runtime(Runtime &&) = delete;
		Runtime &operator=(Runtime const&) = delete;
		Runtime &operator=(Runtime &&) = delete;
	public:

		NETLIB_INL static bool exists();

		/** Makes sure that the socket api is initialised.
			Upon failure, throws an exception. */
		Runtime();
		/** Cleans up the socket api, if this is the last instance existing. */
		~Runtime();
	};
}

#include "Runtime.inl"

#endif