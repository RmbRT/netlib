#ifndef __netlib_async_receive_hpp_defined
#define __netlib_async_receive_hpp_defined

#include "../Socket.hpp"
#include "../x/BufferedConnection.hpp"
#include "BufferedConnection.hpp"

#include <libcr/libcr.hpp>
#include <libcr/util/CVTraits.hpp>
#include <type_traits>

namespace netlib::async
{
	template<class ConditionVariable = cr::mt::ConditionVariable>
	/** Receives data asynchronously.
		Invoke this whenever the socket is able to receive data. */
	TEMPLATE_COROUTINE(Receive, (ConditionVariable), void)
	CR_STATE(
		(StreamSocket &) socket,
		(ConditionVariable &) cv,
		(void *) data,
		(std::size_t) size)

		void cr_destroy() {}
	CR_EXTERNAL

	template<class ConditionVariable = cr::mt::ConditionVariable>
	/** Receives data asynchronously from a buffered connection.
		Invoke this whenever the buffered connection has available input. */
	TEMPLATE_COROUTINE(BufferedReceive, (ConditionVariable), void)
	CR_STATE(
		(x::BufferedConnection &) connection,
		(cr::util::add_cv_pod_t<ConditionVariable> &) cv,
		(void *) data,
		(std::size_t) size)

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
			The buffered connection to receive data with.
		@param[in] data:
			The data to receive.
		@param[in] size:
			The byte count to receive. */
		inline void cr_prepare(
			async::BufferedConnection<CondVar> &connection,
			void * data,
			std::size_t size);

		void cr_destroy() {}
	CR_EXTERNAL
}

#include "Receive.inl"

#endif