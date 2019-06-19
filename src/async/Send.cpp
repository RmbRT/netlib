#include "Send.hpp"
#include <libcr/libcr.hpp>

namespace netlib::async
{
	static void test()
	{
		char data[4096];
		BufferedConnection<> conn { 1024 };
		decltype(conn)::Send send(nullptr, conn, data, sizeof(data));
	}
}