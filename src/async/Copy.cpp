#include "Copy.hpp"

#include <cstring>
#include <cassert>
#include <cinttypes>

namespace netlib::async
{
	Copy::Copy(
		void * output,
		std::size_t size):
		buffer(),
		size(0),
		output(output),
		to_write(size)
	{
	}
	void Copy::add_data(
		void const * buffer,
		std::size_t size)
	{
		assert(!has_leftovers());

		this->buffer = buffer;
		this->size = size;
	}

	CR_IMPL_BEGIN(Copy)
		for(;;)
		{
			if(size >= to_write)
			{
				std::memcpy(output, buffer, to_write);
				reinterpret_cast<std::uintptr_t &>(buffer) += to_write;
				size -= to_write;
				CR_RETURN;
			} else
			{
				std::memcpy(output, buffer, size);
				reinterpret_cast<std::uintptr_t &>(output) += size;
				reinterpret_cast<std::uintptr_t &>(buffer) += size;
				to_write -= size;
				size = 0;
				CR_YIELD;
			}
		}
	CR_IMPL_END_NORETURN
}