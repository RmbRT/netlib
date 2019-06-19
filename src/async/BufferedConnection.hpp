#ifndef __netlib_async_bufferedconnection_hpp_defined
#define __netlib_async_bufferedconnection_hpp_defined

#include "../Poller.hpp"
#include "../x/BufferedConnection.hpp"

#include <libcr/mt/ConditionVariable.hpp>
#include <libcr/sync/ConditionVariable.hpp>

#include <libcr/util/CVTraits.hpp>

namespace netlib::async
{
	template<class ConditionVariable>
	class BufferedSend;
	template<class ConditionVariable>
	class BufferedReceive;

	template<class ConditionVariable = cr::mt::ConditionVariable>
	/** Coroutine-friendlier buffered connection type.
		To be used together with `BufferedSend` and `BufferedReceive`. */
	class BufferedConnection : public x::BufferedConnection
	{
		/** The condition variable signalling available input. */
		ConditionVariable m_can_send;
		/** The condition variable signalling available output. */
		ConditionVariable m_can_receive;

		friend class netlib::async::BufferedSend<cr::util::add_cv_pod_t<ConditionVariable>>;
		friend class netlib::async::BufferedSend<cr::util::remove_cv_pod_t<ConditionVariable>>;
		friend class netlib::async::BufferedReceive<cr::util::add_cv_pod_t<ConditionVariable>>;
		friend class netlib::async::BufferedReceive<cr::util::remove_cv_pod_t<ConditionVariable>>;
	public:
		using x::BufferedConnection::BufferedConnection;

		/** Handles all events with regard to the connection. */
		class PollListener : netlib::PollListener
		{
		public:
			bool operator()(
				Socket * target,
				bool can_read,
				bool can_write,
				bool error) const override;
		};

		/** The coroutine type used to send data with this connection. */
		typedef BufferedSend<ConditionVariable> Send;
		/** The coroutine type used to receive data with this connection. */
		typedef BufferedReceive<ConditionVariable> Receive;
	};
}

#include "BufferedConnection.inl"

#endif