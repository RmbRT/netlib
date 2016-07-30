#include <utility>

namespace netlib
{
	namespace x
	{
		Connection::Connection(
			netlib::StreamSocket &&socket):
			netlib::StreamSocket(std::move(socket))
		{
		}

		bool Connection::send(
			std::vector<byte_t> const& data)
		{
			return send(data.data(), data.size());
		}
	}
}