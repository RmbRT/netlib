namespace netlib::x
{
	std::size_t BufferedConnection::send(
		void const * data,
		std::size_t size)
	{
		return m_output.append(data, size);
	}

	std::size_t BufferedConnection::receive(
		void * data,
		std::size_t size)
	{
		return m_input.consume(data, size);
	}
}