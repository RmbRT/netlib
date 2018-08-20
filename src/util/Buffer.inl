namespace netlib::util
{
	std::size_t Buffer::capacity() const noexcept
	{
		return m_buffer.size();
	}

	std::size_t Buffer::free_space() const noexcept
	{
		return m_buffer.size() - m_size;
	}

	void * Buffer::data() noexcept
	{
		return m_buffer.data() + m_begin;
	}

	void const * Buffer::data() const noexcept
	{
		return m_buffer.data() + m_begin;
	}

	std::size_t Buffer::size() const noexcept
	{
		return m_size;
	}

	bool Buffer::full() const noexcept
	{
		return capacity() == m_size;
	}

	bool Buffer::empty() const noexcept
	{
		return !m_size;
	}

	std::size_t Buffer::continuous_data() const noexcept
	{
		std::size_t to_edge = capacity() - m_begin;
		if(m_size > to_edge)
			return m_size - to_edge;
		else
			return m_size;
	}

	std::size_t Buffer::continuous_free_space() const noexcept
	{
		std::size_t end = m_begin + m_size;
		if(end >= capacity())
			return free_space();
		else
			return capacity() - end;
	}

	bool Buffer::wrapping() const
	{
		return m_begin + m_size > capacity();
	}
}