#include <cassert>
namespace netlib::async
{
	template<class ConditionVariable>
	CR_IMPL(Receive<ConditionVariable>)
		if(size
		&& (socket->async() || !socket->pending()))
			CR_AWAIT(cv->wait());

		while(size)
		{
			// Make sure this coroutine is used as intended.
			assert(socket->async() || socket->pending());

			// Need a separate scope for the local variable.
			{
				std::size_t count = socket->recv(data, size);
				size -= count;
				reinterpret_cast<std::uint8_t const *&>(data) += count;

				if(!count
				&& !(socket->async() && Socket::would_block()))
				{
					CR_THROW;
				}
			}

			// Only yield if there is still more data to come.
			if(size)
				CR_AWAIT(cv->wait());
		}
	CR_IMPL_END

	template<class ConditionVariable>
	template<class CondVar, class>
	void BufferedReceive<ConditionVariable>::cr_prepare(
		async::BufferedConnection<CondVar> &connection,
		void * data,
		std::size_t size)
	{
		cr_prepare(
			connection,
			connection.m_can_receive,
			data,
			size);
	}

	template<class ConditionVariable>
	CR_IMPL(BufferedReceive<ConditionVariable>)
		// In the beginning, only wait if no data can be received.
		if(size && connection->input().empty())
			CR_AWAIT(cv->wait());

		while(size)
		{
			// Make sure this coroutine is used as intended.
			assert(!connection->input().empty());

			// Need a separate scope for the local variable.
			{
				std::size_t count = connection->receive(data, size);
				size -= count;
				reinterpret_cast<std::uint8_t *&>(data) += count;
			}

			// Only yield if there is still more data to come.
			if(size)
				CR_AWAIT(cv->wait());
		}
	CR_IMPL_END
}