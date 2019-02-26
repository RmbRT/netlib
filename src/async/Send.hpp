#ifndef __netlib_async_send_hpp_defined
#define __netlib_async_send_hpp_defined

#include "../Socket.hpp"
#include "../x/BufferedConnection.hpp"
#include "BufferedConnection.hpp"

#include <libcr/libcr.hpp>
#include <libcr/util/CVTraits.hpp>

namespace netlib::async
{
	template<class ConditionVariable = cr::mt::ConditionVariable>
	/** Sends data asynchronously.
		Invoke this whenever the socket is able to send data. */
	TEMPLATE_COROUTINE(Send, (ConditionVariable), void)
		/** Whether an error occurred. */
		inline bool error() const;
	CR_STATE
		StreamSocket * socket;
		cr::util::add_cv_pod_t<ConditionVariable> * cv;
		std::uint8_t const * data;
		std::size_t size;

		bool m_error;

		/** Initialises the coroutine.
		@param[in] socket:
			The socket to send data with.
		@param[in] cv:
			The condition variable that signals when data can be sent.
		@param[in] data:
			The data to send.
		@param[in] size:
			The byte count to send. */
		void cr_prepare(
			StreamSocket &socket,
			cr::util::add_cv_pod_t<ConditionVariable> &cv,
			void const * data,
			std::size_t size);
	CR_EXTERNAL


	template<class ConditionVariable>
	/** Sends data asynchronously to a buffered connection.
		Invoke this whenever the buffered connection is able to send data. */
	TEMPLATE_COROUTINE(BufferedSend, (ConditionVariable), void)
	CR_STATE
		cr::util::add_cv_pod_t<ConditionVariable> * cv;
		x::BufferedConnection * connection;
		std::uint8_t const * data;
		std::size_t size;
		/** Initialises the coroutine.
		@param[in] connection:
			The connection to send data with.
		@param[in] cv:
			The condition variable that signals when data can be sent.
		@param[in] data:
			The data to send.
		@param[in] size:
			The byte count to send. */
		void cr_prepare(
			x::BufferedConnection &connection,
			cr::util::add_cv_pod_t<ConditionVariable> &cv,
			void const * data,
			std::size_t size);

		template<
			class CondVar,
			class = typename std::enable_if<
				std::is_same<
					cr::util::add_cv_pod_t<ConditionVariable>,
					cr::util::add_cv_pod_t<CondVar>
					>::value
				>::type
			>
		/** Initialises the coroutine.
		@tparam CondVar:
			The condition variable type used by the buffered connection. This type must be the same as this coroutine's template parameter (ignoring whether it is the POD version or not).
		@param[in] connection:
			The connection to send data with.
		@param[in] data:
			The data to send.
		@param[in] size:
			The byte count to send. */
		void cr_prepare(
			async::BufferedConnection<CondVar> &connection,
			void const * data,
			std::size_t size);
	CR_EXTERNAL
}

#include "Send.inl"

#endif