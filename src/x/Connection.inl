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

		bool Connection::send_all(
			std::string const& data)
		{
			return send(data.data(), data.size());
		}

		bool Connection::send_all(
			std::vector<byte_t> const& data)
		{
			return send(data.data(), data.size());
		}
	}
}