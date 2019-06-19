#include "Receive.hpp"
#include <libcr/libcr.hpp>

namespace netlib::async
{
	static void test()
	{
		char data[4096];
		BufferedConnection<> conn { 1024 };
		decltype(conn)::Receive send(nullptr, conn, data, sizeof(data));
	}
}