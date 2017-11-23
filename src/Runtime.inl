namespace netlib
{
	bool Runtime::exists()
	{
		return s_instances.load(std::memory_order_relaxed) != 0;
	}
}