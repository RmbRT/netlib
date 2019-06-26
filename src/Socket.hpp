/** @file Socket.hpp
	Contains the Socket classes and functions. This is the 'heart' of the netlib. Main functionality resolving around sockets is all declared in here. */
#ifndef __netlib_socket_hpp_defined
#define __netlib_socket_hpp_defined

#include "defines.hpp"
#include "Protocol.hpp"
#include "SocketAddress.hpp"

#include <libcr/mt/ConditionVariable.hpp>

namespace netlib
{
	/** The type / usage of a socket. */
	enum class SocketType
	{
		/** Stream sockets. */
		kStream,
		/** Datagram sockets. */
		kDatagram,
		/** Raw sockets. */
		kRaw,
		/** RDM sockets. */
		kRDM, kReliablyDeliveredMessage = kRDM,
		/** SPS sockets. */
		NETLIB_LAST(kSPS), kSequencedPacketStream = kSPS
	};

	/** Constants used for the Socket::shutdown command. */
	enum class Shutdown
	{
		/** Stop receiving. */
		kReceive,
		/** Stop sending. */
		kSend,
		/** Stop both. */
		kBoth
	};

	/** Error values. */
	enum class Status
	{
		/** Operation was successful. */
		kSuccess,
		/** Operation would block. */
		kNotReady,
		/** Operation is in progress. */
		kInProgress,
		/** Real error occurred. */
		kError
	};

	// Forward declaration.
	class Poller;
	class PollEvent;

	namespace detail
	{
		typedef int socket_t;
	}

	/** Basic Socket class.
		Represents a generic Socket and contains functions that all types of Sockets have. */
	class Socket
	{
		friend class ::netlib::Poller;
		friend struct ::netlib::PollEvent;
	protected:
		/** The socket address. */
		SocketAddress m_address;
		/** The protocol used by the socket. */
		Protocol m_protocol;
		/** The socket's type. */
		SocketType m_type;
		/** The socket handle. */
		detail::socket_t m_socket;
		/** Notified when input is pending. */
		cr::mt::ConditionVariable m_input;
		/** Notified when output is possible. */
		cr::mt::ConditionVariable m_output;
	public:
		/** Creates an empty socket. */
		Socket();
		/** Creates and allocates a socket of the given characteristics.
		@param[in] family:
			The address family of the socket.
		@param[in] type:
			The type of the socket.
		@param[in] protocol:
			The socket's protocol. */
		Socket(
			AddressFamily family,
			SocketType type,
			Protocol protocol = Protocol::kDefault);

		/** Creates and allocates a socket of the given characteristics.
			Does not connect to the given address, but saves it.
		@param[in] address:
			Indicates the address family used by the socket.
		@param[in] type:
			The socket's type.
		@param[in] protocol:
			The socket's protocol. */
		Socket(
			SocketAddress const& address,
			SocketType type,
			Protocol protocol = Protocol::kDefault);

		/** Closes the socket, if it exists. */
		~Socket();

		/* Socket does not support copying. */
		Socket(Socket const&) = delete;
		Socket&operator=(Socket const&) = delete;

		/* Sockets can only be moved. */
		Socket(Socket &&);
		Socket &operator=(Socket &&);

		/** Closes the socket. */
		void close();

		/** Tries to bind the socket to `address`.
		@param[in] address:
			The address to bind the socket to.
		@param[in] reuse_address:
			Whether to attempt to reuse the address.
		@return
			Whether the socket could be bound to `address`. */
		bool bind(
			SocketAddress const& address,
			bool reuse_address = false);

		/** Tries to connect to `address`.
			On datagram-based sockets, sets `address` as the default recipient and sender for `recv()` and `send()`.
		@param[in] address:
			The address to connect to.
		@return
			Whether the operation succeeded. */
		Status connect(
			SocketAddress const& address);

		/** Shuts down the socket.
		@param[in] what:
			What aspect to shutdown.
		@return
			Whether the operation succeeded. */
		Status shutdown(
			Shutdown what);

		/** Sends at most `size` bytes of `data`.
		@param[in] data:
			The data to send.
		@param[in] size:
			How many bytes to send at most.
		@param[out] sent:
			On success, the number of bytes sent.
		@return
			Whether the operation succeeded. */
		Status send(
			void const* data,
			std::size_t size,
			std::size_t &sent);
		/** Receives at most `size` bytes into `data`.
		@param[out] data:
			Where to receive the incoming data into.
		@param[in] size:
			How many bytes to receive at most.
		@param[out] received:
			On success, the number of bytes received.
		@return
			Whether the operation succeeded. */
		Status recv(
			void * data,
			std::size_t size,
			std::size_t &received);

		/** Sends at most `size` bytes of `data` to the given address.
		@param[in] data:
			The data to send.
		@param[in] size:
			How many bytes to send.
		@param[in] to:
			The address to send the data to.
		@param[out] sent:
			On success, the number of bytes sent.
		@return
			Whether the operation succeeded. */
		Status sendto(
			void const * data,
			std::size_t size,
			SocketAddress const& to,
			std::size_t &sent);

		/** Receives at most `size` bytes into `data` from the given address.
		@param[out] data:
			Where to receive the incoming data into.
		@param[in] size:
			How many bytes to receive at most.
		@param[out] from:
			On success, where the data was received from.
		@param[out] received:
			On success, the number of bytes received.
		@return
			Whether the operation succeeded. */
		Status recvfrom(
			void * data,
			std::size_t size,
			SocketAddress &from,
			std::size_t &received);

		/** Whether this Socket exists. */
		NETLIB_INL bool exists() const;
		/** Same as `exists()`. */
		NETLIB_INL operator bool() const;

		/** Returns the address of this Socket. */
		NETLIB_INL SocketAddress const& address() const;
		/** Returns the protocol of this Socket. */
		NETLIB_INL Protocol protocol() const;
		/** Returns the type of this Socket. */
		NETLIB_INL SocketType type() const;
	};

	/** Represents a stream socket. */
	class StreamSocket : public Socket
	{
	public:
		/** Creates a stream socket for the requested address family.
		@param[in] family:
			The requested address family.
			Must be either IPv4 and IPv6. */
		explicit StreamSocket(
			AddressFamily family);

		StreamSocket() = default;
		StreamSocket(StreamSocket const&) = delete;
		StreamSocket(StreamSocket &&) = default;

		StreamSocket &operator=(StreamSocket const&) = delete;
		StreamSocket &operator=(StreamSocket &&) = default;

		/** Listens on the currently bound address.
		@return
			Whether it succeeded. */
		bool listen();
		/** Accepts an incoming connection.
		@param[out] out:
			The socket to hold the incoming connection.
		@return
			Whether the operation succeeded. */
		Status accept(
			StreamSocket &out);
	};

	/** Represents a datagram socket. */
	class DatagramSocket : public Socket
	{
	public:
		/** Creates a datagram socket for the requested address family. */
		DatagramSocket(
			AddressFamily);
		DatagramSocket &operator=(DatagramSocket &&) = default;
		DatagramSocket(DatagramSocket &&) = default;

		DatagramSocket &operator=(DatagramSocket const&) = delete;
		DatagramSocket(DatagramSocket const&) = delete;
	};
}

#include "Socket.inl"

#endif