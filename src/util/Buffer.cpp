#include "Buffer.hpp"

#include <cstring>

namespace netlib::util
{
	Buffer::Buffer(
		std::size_t capacity):
		m_buffer(capacity),
		m_begin(0),
		m_size(0)
	{
	}

	std::size_t Buffer::add(
		std::size_t size) noexcept
	{
		std::size_t free = free_space();
		if(size > free)
			size = free;

		m_size += size;

		return size;
	}

	std::size_t Buffer::append(
		void const * data,
		std::size_t size) noexcept
	{
		std::size_t free = free_space();
		if(size > free)
			size = free;

		std::size_t end_index = m_begin + m_size;

		if(end_index >= capacity())
		{
			end_index -= capacity();
			std::memcpy(
				m_buffer.data() + end_index,
				data,
				size);
		} else if(end_index + size > capacity())
		{
			std::size_t before_wrap = capacity() - end_index;
			std::memcpy(
				m_buffer.data() + end_index,
				data,
				before_wrap);

			std::memcpy(
				m_buffer.data(),
				static_cast<std::uint8_t const *>(data) + before_wrap,
				size - before_wrap);
		} else
		{
			std::memcpy(
				m_buffer.data() + end_index,
				data,
				size);
		}

		add(size);

		return size;
	}

	std::size_t Buffer::remove(
		std::size_t size) noexcept
	{
		if(size >= m_size)
		{
			size = m_size;
			m_begin = 0;
			m_size = 0;
		} else
		{
			m_size -= size;
			m_begin += size;

			if(m_begin >= capacity())
				m_begin -= capacity();
		}
		return size;
	}

	std::size_t Buffer::consume(
		void * data,
		std::size_t size) noexcept
	{
		if(size > m_size)
			size = m_size;

		std::size_t first = continuous_data();

		if(first > size)
			first = size;

		std::memcpy(
			data,
			m_buffer.data() + m_begin,
			first);

		if(size > first)
			std::memcpy(
				static_cast<std::uint8_t *>(data) + first,
				m_buffer.data(),
				size - first);

		remove(size);

		return size;
	}

	void Buffer::clear() noexcept
	{
		m_begin = 0;
		m_size = 0;
	}

	void * Buffer::end() noexcept
	{
		if(wrapping())
			return m_buffer.data() + (m_begin + m_size - capacity());
		else
			return m_buffer.data() + (m_begin + m_size);
	}

	void const * Buffer::end() const noexcept
	{
		if(wrapping())
			return m_buffer.data() + (m_begin + m_size - capacity());
		else
			return m_buffer.data() + (m_begin + m_size);
	}
}