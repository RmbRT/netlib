#include <cassert>
namespace netlib::async
{
	template<class ConditionVariable>
	CR_IMPL(Send<ConditionVariable>)
		while(size)
		{
			// Always wait before trying to send.
			CR_AWAIT(cv->wait());
			// Need a separate scope for the local variable.
			{
				std::size_t count = socket->send(data, size);
				size -= count;
				reinterpret_cast<std::uint8_t const *&>(data) += count;
				if(!count
				&& !(socket->async() && Socket::would_block()))
				{
					CR_THROW;
				}
			}

		}
	CR_IMPL_END

	template<class ConditionVariable>
	template<class CondVar, class>
	void BufferedSend<ConditionVariable>::cr_prepare(
		async::BufferedConnection<CondVar> &connection,
		void const * data,
		std::size_t size)
	{
		cr_prepare(
			connection.m_can_send,
			connection,
			data,
			size);
	}

	template<class ConditionVariable>
	CR_IMPL(BufferedSend<ConditionVariable>)
		if(size && connection->output().full())
			CR_AWAIT(cv->wait());

		while(size)
		{
			// Make sure this coroutine is used as intended.
			assert(!connection->output().full());

			// Need a separate scope for the local variable.
			{
				std::size_t count = connection->send(data, size);
				size -= count;
				reinterpret_cast<std::uint8_t const *&>(data) += count;
			}

			// Only yield if there is still more data to come.
			if(size)
				CR_AWAIT(cv->wait());
		}
	CR_IMPL_END
}