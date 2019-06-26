namespace netlib::x
{
	constexpr BufferedConnection * BufferedConnection::cast_from_base(
		Socket * base)
	{
		return static_cast<BufferedConnection *>(base);
	}

	constexpr BufferedConnection const * BufferedConnection::cast_from_base(
		Socket const * base)
	{
		return static_cast<BufferedConnection const *>(base);
	}

	util::Buffer const& BufferedConnection::input() const noexcept
	{
		return m_input;
	}

	util::Buffer const& BufferedConnection::output() const noexcept
	{
		return m_output;
	}
}