namespace netlib::async
{
	void const * Copy::leftover_data() const
	{
		return buffer;
	}

	std::size_t Copy::leftover_size() const
	{
		return size;
	}

	bool Copy::has_leftovers() const
	{
		return !size;
	}
}