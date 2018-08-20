namespace netlib
{
	bool Poller::empty() const
	{
		return m_watch_list.empty();
	}

	template<class T, class>
	bool Poller::watch(
		T * object,
		PollListener * listener,
		bool read,
		bool write)
	{
		return watch(
			static_cast<Socket *>(object),
			listener,
			read,
			write);
	}

	void Poller::reserve_additional(
		std::size_t size)
	{
#ifdef NETLIB_EPOLL
		reserve(m_event_list_size + size);
#else
		reserve(m_sockets.size() + size);
#endif
	}
}