/** @file Connection.hpp
	Contains the netlib::x::Connection class used for handling connections. */
#ifndef __netlib_x_connection_hpp_defined
#define __netlib_x_connection_hpp_defined

#include "../Socket.hpp"
#include "../defines.hpp"

#include <cinttypes>
#include <vector>
#include <string>

namespace netlib::x
{
	/** Used for raw data transfer. */
	typedef std::uint8_t byte_t;

	/** A network connection.
		This class extends the behaviour of the netlib::StreamSocket class to create more comfortably usable functions for more efficient connection handling. Whether a connection is open can be retrieved via `Connection::exists`. Establishing a connection can be done via `Connection::connect`. This class can be used with `netlib::x::ConnectionListener`. */
	class Connection : private StreamSocket
	{
		friend class ::netlib::Poller;
	public:
		using StreamSocket::operator bool;
		using StreamSocket::exists;
		using StreamSocket::close;
		using StreamSocket::shutdown;
		using StreamSocket::connect;
		using StreamSocket::pending;
		using StreamSocket::async;
		using StreamSocket::set_async;
		using StreamSocket::send;

		/** Creates a closed connection. */
		Connection() = default;
		/** Creates a connection with the given socket.
		@param[in] socket:
			The socket to use for the connection. */
		NETLIB_INL Connection(
			netlib::StreamSocket && socket);

		Connection(Connection &&) = default;
		Connection & operator=(Connection&&) = default;

		Connection(Connection const&) = delete;
		Connection & operator=(Connection const&) = delete;

		/** Receives a chunk of data until none is pending, or until the given maximum count.
			This call cannot block.
			In blocking sockets, calls pending().
		@param[out] out:
			The buffer storing the received data.
		@param[in] max_count:
			The maximum byte count to receive.
		@return
			Whether the data was successfully received. */
		bool receive_pending(
			std::vector<byte_t> & out,
			std::size_t max_count);

		/** Receives a chunk of data until none is pending, or until the given maximum count.
			This call cannot block.
		@param[out] out:
			The buffer storing the received data.
		@param[in] max_count:
			The maximum byte count to receive.
		@return
			How much data was received. */
		std::size_t receive_pending(
			void * out,
			std::size_t max_count);

		/** Receives exactly `count` bytes, or fails.
			This call may block.
		@param[out] out:
			The buffer storing the received data.
		@param[in] count:
			The expected number of bytes.
		@return
			Whether the receiving succeeded. */
		bool receive_all(
			void * out,
			std::size_t count);

		/** Receives exactly `count` bytes, or fails.
			This call may block.
		@param[out] out:
			The buffer storing the received data.
		@param[in] count:
			The expected number of bytes.
		@return
			Whether the receiving succeeded. */
		bool receive_all(
			std::vector<byte_t> & out,
			std::size_t count);

		/** Sends data.
			This function is for use with other container classes, or for static data.
		@param[in] data:
			The data to send.
		@param[in] size:
			The byte count of the data.
		@return
			Whether the sending succeeded. */
		bool send_all(
			void const * data,
			std::size_t size);

		/** Sends data from a string.
			Equal to `send(data, data.size())`.
		@param[in] data:
			The data to send.
		@return
			Whether the sending succeeded. */
		NETLIB_INL bool send_all(
			std::string const& data);

		/** Sends data from a vector.
		@param[in] data:
			The data to send.
		@return
			Whether the sending succeeded. */
		NETLIB_INL bool send_all(
			std::vector<byte_t> const& data);
	};
}

#include "Connection.inl"

#endif