#include <cassert>
namespace netlib::async
{
	template<class ConditionVariable>
	bool Receive<ConditionVariable>::error() const
	{
		return m_error;
	}

	template<class ConditionVariable>
	void Receive<ConditionVariable>::cr_prepare(
		StreamSocket &socket,
		cr::util::add_cv_pod_t<ConditionVariable> &watch,
		void * data,
		std::size_t size)
	{
		this->socket = &socket;
		this->data = reinterpret_cast<std::uint8_t *>(data);
		this->size = size;
		this->m_error = false;
	}

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
				data += count;

				if(!count
				&& !(socket->async() && Socket::would_block()))
				{
					m_error = true;
					break;
				}
			}

			// Only yield if there is still more data to come.
			if(size)
				CR_AWAIT(cv->wait());
		}
	CR_IMPL_END

	template<class ConditionVariable>
	void BufferedReceive<ConditionVariable>::cr_prepare(
		x::BufferedConnection &connection,
		cr::util::add_cv_pod_t<ConditionVariable> &watch,
		void * data,
		std::size_t size)
	{
		this->cv = &watch;
		this->connection = &connection;
		this->data = reinterpret_cast<std::uint8_t *>(data);
		this->size = size;
	}

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
				data += count;
			}

			// Only yield if there is still more data to come.
			if(size)
				CR_AWAIT(cv->wait());
		}
	CR_IMPL_END
}