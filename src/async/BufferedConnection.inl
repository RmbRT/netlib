namespace netlib::async
{
	template<class ConditionVariable>
	bool BufferedConnection<ConditionVariable>::PollListener::operator()(
		Socket * target,
		bool can_read,
		bool can_write,
		bool error) const
	{
		BufferedConnection<ConditionVariable> * connection = (BufferedConnection<ConditionVariable> *) target;

		if(can_read)
		{
			bool was_empty = connection->input().empty();
			if(!connection->input().full())
				if(connection->receive_some() && was_empty)
					connection->m_can_receive.notify_one();
		}

		if(can_write)
		{
			bool was_full = connection->output().full();
			if(!connection->output().empty())
				if(connection->flush_some() && was_full)
					connection->m_can_send.notify_one();
		}

		if(error)
		{
		}
	}
}