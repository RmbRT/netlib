namespace netlib
{
	std::size_t Poller::watched() const
	{
		return m_sockets.size();
	}
}