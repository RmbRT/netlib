#ifndef __netlib_async_copy_hpp_defined
#define __netlib_async_copy_hpp_defined

#include <libcr/libcr.hpp>

namespace netlib::async
{
	/** Coroutine that copies data from an event-based stream.
		Before invoking, call `add_data()` to provide input. This coroutine is best used to read from an input buffer, to reduce the amount of system calls. */
	COROUTINE(Copy)
		/** Prepares an execution instance.
		@param[in] output:
			Where to output the data to.
		@param[in] size:
			How many bytes to copy. */
		void prepare(
			void * output,
			std::size_t size);

		/** Prepares an execution instance.
		@param[in] output:
			Where to output the data to.
		@param[in] size:
			How many bytes to copy.
		@param[in] parent:
			The parent coroutine. */
		void prepare(
			void * output,
			std::size_t size,
			cr::Coroutine * parent);

		/** Adds new data for the coroutine's next iteration.
			This function must only be called if there is no leftover data.
		@param[in] buffer:
			The new data.
		@param[in] size:
			How many bytes of data are available. */
		void add_data(
			void const * buffer,
			std::size_t size);

		/** Returns the unused part of the buffer.
		@return
			The beginning of the leftover data. If there is no leftover data, points to the end of the buffer. */
		void const * leftover_data() const;
		/** Returns the size of the unused part of the buffer.
		@return
			The byte count of the buffer's leftover data. */
		inline std::size_t leftover_size() const;
		/** Detects whether there are leftovers. */
		inline bool has_leftovers() const;
	CR_STATE
		/** The input buffer. */
		void const * buffer;
		/** How many unread bytes are in the buffer. */
		std::size_t size;
		/** Where to output the data to. */
		void * output;
		/** How many bytes to write. */
		std::size_t to_write;
	CR_EXTERNAL
}

#include "Copy.inl"

#endif