#ifndef __netlib_util_buffer_hpp_defined
#define __netlib_util_buffer_hpp_defined

#include <cinttypes>
#include <vector>

namespace netlib::util
{
	/** Lightweight circular queue buffer class.
		The buffer has a fixed size and can be used to reduce the amount of system calls used in communication. Note that because the buffer wraps around its edges, the data might not be continuous, in which case it has to be retrieved in two rounds. The `continuous_data()` can be used to detect how much data is available at once. Use `data()` to access the beginning of the stored data. */
	class Buffer
	{
		/** The internal data buffer. */
		std::vector<std::uint8_t> m_buffer;
		/** Where the contents start. */
		std::size_t m_begin;
		/** How much data is in the buffer. */
		std::size_t m_size;
	public:
		/** Creates a buffer with the requested capacity. */
		explicit Buffer(
			std::size_t capacity);

		/** The buffer's capacity. */
		inline std::size_t capacity() const noexcept;

		/** How many bytes of free space are in the buffer. */
		inline std::size_t free_space() const noexcept;

		/** The buffer's contents. */
		inline void * data() noexcept;
		/** The buffer's contents. */
		inline void const * data() const noexcept;
		/** The current size of the buffer contents.
			Prefer `continuous_data()` for accessing data. */
		inline std::size_t size() const noexcept;
		/** Whether the buffer is full. */
		inline bool full() const noexcept;
		/** Whether the buffer is empty. */
		inline bool empty() const noexcept;

		/** Retrieves the amount of continuous data at the beginning of the buffer.
		@return
			If the buffer is wrapping around its edge, only the data size until the edge is returned. */
		inline std::size_t continuous_data() const noexcept;

		/** Adds up to `size` bytes to the end of the buffer.
			This method does not actually modify the buffer's contents.
		@param[in] size:
			How many bytes to write to the buffer.
		@return
			How many bytes were added to the buffer. */
		std::size_t add(
			std::size_t size) noexcept;

		/** Copies and adds up to `size` bytes to the end of the buffer.
		@param[in] data:
			What data to write to the buffer.
		@param[in] size:
			How many bytes to write to the buffer.
		@return
			How many bytes were added to the buffer. */
		std::size_t append(
			void const * data,
			std::size_t size) noexcept;

		/** Removes up to `size` bytes from the beginning of the buffer.
		@param[in] size:
			How many bytes to remove.
		@return
			How many bytes were actually removed. */
		std::size_t remove(
			std::size_t size) noexcept;

		/** Copies and removes up to `size` bytes from the beginning of the buffer.
		@param[out] data:
			The data to copy from the buffer.
		@param[in] size:
			How many bytes to consume.
		@return
			How many bytes were actually consumed. */
		std::size_t consume(
			void * data,
			std::size_t size) noexcept;

		/** The end of the buffer contents, and beginning of the free space. */
		void * end() noexcept;
		/** The end of the buffer contents, and beginning of the free space. */
		void const * end() const noexcept;
		/** Whether the buffer is wrapping its contents around the edges. */
		inline bool wrapping() const;
	};
}

#include "Buffer.inl"

#endif