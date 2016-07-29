namespace netlib
{
	bool Runtime::exists()
	{
		return *lock::ReadLock<std::size_t>(s_instances) != 0;
	}
}