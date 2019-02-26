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
		/** Whether an error occurred. */
		inline bool error() const;
	CR_STATE
		ConditionVariable * cv;
		StreamSocket * socket;
		std::uint8_t * data;
		std::size_t size;

		bool m_error;

		/** Initialises the coroutine.
		@param[in] socket:
			The socket to receive data with.
		@param[in] cv:
			The condition variable that signals when data is available.
		@param[in] data:
			The data to receive.
		@param[in] size:
			The byte count to receive. */
		void cr_prepare(
			StreamSocket &socket,
			cr::util::add_cv_pod_t<ConditionVariable> &cv,
			void * data,
			std::size_t size);
	CR_EXTERNAL

	template<class ConditionVariable = cr::mt::ConditionVariable>
	/** Receives data asynchronously from a buffered connection.
		Invoke this whenever the buffered connection has available input. */
	TEMPLATE_COROUTINE(BufferedReceive, (ConditionVariable), void)
	CR_STATE
		cr::util::add_cv_pod_t<ConditionVariable> * cv;
		x::BufferedConnection * connection;
		std::uint8_t * data;
		std::size_t size;

		/** Initialises the coroutine.
		@param[in] connection:
			The buffered connection to receive data with.
		@param[in] cv:
			The condition variable that signals when data is available.
		@param[in] data:
			The data to receive.
		@param[in] size:
			The byte count to receive. */
		void cr_prepare(
			x::BufferedConnection &connection,
			cr::util::add_cv_pod_t<ConditionVariable> &cv,
			void * data,
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
			The buffered connection to receive data with.
		@param[in] data:
			The data to receive.
		@param[in] size:
			The byte count to receive. */
		inline void cr_prepare(
			async::BufferedConnection<CondVar> &connection,
			void * data,
			std::size_t size);
	CR_EXTERNAL
}

#include "Receive.inl"

#endif