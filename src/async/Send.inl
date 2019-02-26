#include <cassert>
namespace netlib::async
{
	template<class ConditionVariable>
	bool Send<ConditionVariable>::error() const
	{
		return m_error;
	}

	template<class ConditionVariable>
	void Send<ConditionVariable>::cr_prepare(
		StreamSocket &socket,
		cr::util::add_cv_pod_t<ConditionVariable> &cv,
		void const * data,
		std::size_t size)
	{
		this->socket = &socket;
		this->cv = &cv;
		this->data = reinterpret_cast<std::uint8_t const *>(data);
		this->size = size;
		m_error = false;
	}

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
				data += count;

				if(!count
				&& !(socket->async() && Socket::would_block()))
				{
					m_error = true;
					CR_RETURN;
				}
			}

		}
	CR_IMPL_END

	template<class ConditionVariable>
	void BufferedSend<ConditionVariable>::cr_prepare(
		x::BufferedConnection &connection,
		cr::util::add_cv_pod_t<ConditionVariable> &cv,
		void const * data,
		std::size_t size)
	{
		this->connection = &connection;
		this->cv = &cv;
		this->data = reinterpret_cast<std::uint8_t const *>(data);
		this->size = size;
	}

	template<class ConditionVariable>
	template<class CondVar, class>
	void BufferedSend<ConditionVariable>::cr_prepare(
		async::BufferedConnection<CondVar> &connection,
		void const * data,
		std::size_t size)
	{
		cr_prepare(
			connection,
			connection.m_can_send,
			data,
			size);
	}

	template<class ConditionVariable>
	CR_IMPL(BufferedSend<ConditionVariable>)
		while(size)
		{
			// Make sure this coroutine is used as intended.
			assert(!connection->output().full());

			// Need a separate scope for the local variable.
			{
				std::size_t count = connection->send(data, size);
				size -= count;
				data += count;
			}

			// Only yield if there is still more data to come.
			if(size)
				CR_AWAIT(cv->wait());
		}
	CR_IMPL_END
}