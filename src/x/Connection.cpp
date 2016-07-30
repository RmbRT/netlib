#include "Connection.hpp"
#include <utility>
#include <cstdio>

namespace netlib
{
	namespace x
	{
		bool Connection::receive(
			std::vector<byte_t> & out)
		{
			out.clear();
			// for thread safety.
			static thread_local std::vector<byte_t> buffer(4096);

			while(pending())
			{
				size_t count = recv(buffer.data(), buffer.size());
				// error?
				if(!count)
					return false;

				out.insert(out.end(), buffer.begin(), buffer.end());
			}

			return true;
		}

		bool Connection::send(
			void const * data,
			std::size_t size)
		{
			byte_t const * to_send =
				static_cast<byte_t const *>(
					data);
			while(size)
			{
				std::size_t sent = send(
					to_send,
					size);
				if(!sent)
					return false;
				size -= sent;
				to_send += sent;
			}

			return true;
		}
	}
}