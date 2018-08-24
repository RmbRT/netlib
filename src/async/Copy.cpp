#include "Copy.hpp"

#include <cstring>
#include <cassert>
#include <cinttypes>

namespace netlib::async
{
	void Copy::prepare(
		void * output,
		std::size_t size)
	{
		LibCrBase::prepare();
		size = 0;
		this->output = output;
		to_write = size;
	}

	void Copy::prepare(
		void * output,
		std::size_t size,
		cr::Coroutine * parent)
	{
		LibCrBase::prepare(parent);
		size = 0;
		this->output = output;
		to_write = size;
	}

	void Copy::add_data(
		void const * buffer,
		std::size_t size)
	{
		assert(!has_leftovers());

		this->buffer = buffer;
		this->size = size;
	}

	CR_IMPL(Copy)
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
	CR_IMPL_END
}