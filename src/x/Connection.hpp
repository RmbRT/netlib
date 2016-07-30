#ifndef __netlib_x_connection_hpp_defined
#define __netlib_x_connection_hpp_defined

#include "../Socket.hpp"
#include "../defines.hpp"

#include <cinttypes>
#include <vector>

namespace netlib
{
	namespace x
	{
		/** Used for raw data transfer. */
		typedef std::uint8_t byte_t;

		/** A network connection.
			This class extends the behaviour of the netlib::StreamSocket class to create more comforatbly usable functions for more efficient connection handling. Whether a connection is open can be retrieved via `Connections::exists`. Establishing a connection can be done via `Connection::connect`. This class can be used with `netlib::x::ConnectionListener`. */
		class Connection : private StreamSocket
		{
		public:
			using StreamSocket::operator bool;
			using StreamSocket::exists;
			using StreamSocket::close;
			using StreamSocket::shutdown;
			using StreamSocket::connect;

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


			/** Receives a chunk of data until none is pending.
			@param[out] out:
				The buffer storing the received data.
			@return
				Whether the data was successfully received. */
			bool receive(
				std::vector<byte_t> & out);


			/** Sends data.
				This function is for use with other container classes, or for static data.
			@param[in] data:
				The data to send.
			@param[in] size:
				The byte count of the data.
			@return
				Whether the sending succeeded. */
			bool send(
				void const * data,
				std::size_t size);
			/** Sends data.
			@param[in] data:
				The data to send.
			@return
				Whether the sending succeeded. */
			NETLIB_INL bool send(
				std::vector<byte_t> const& data);
		};
	}
}

#include "Connection.inl"

#endif