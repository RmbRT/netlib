#ifndef __netlib_poller_hpp_defined
#define __netlib_poller_hpp_defined

#include "Socket.hpp"

namespace netlib
{
	class Poller
	{
		void * m_data;
		std::size_t m_sockets;
		std::uintptr_t m_poller;
	public:
		Poller();

		void watch(
			Socket const * const * sockets,
			std::size_t count);

		void poll();
	};
}

#endif