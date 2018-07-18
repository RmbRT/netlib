/** @file Socket.hpp
	Contains the Socket classes and functions. This is the 'heart' of the netlib. Main functionality resolving around sockets is all declared in here. */
#ifndef __netlib_socket_hpp_defined
#define __netlib_socket_hpp_defined

#include "defines.hpp"
#include "Protocol.hpp"
#include "SocketAddress.hpp"
#include "Poller.hpp"

namespace netlib
{

	class Socket;

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

#ifdef NETLIB_BUILD
	/** Converts a socket type to its corresponding native socket api constant. */
	int to_native_api(
		SocketType type);
#endif

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

	/** Basic Socket class.
		Represents a generic Socket and contains functions that all types of Sockets have. */
	class Socket
	{
		friend class ::netlib::Poller;
	protected:
		/** The socket address. */
		SocketAddress m_address;
		/** The protocol used by the socket. */
		Protocol m_protocol;
		/** The socket's type. */
		SocketType m_type;
		/** The socket handle. */
		std::uintptr_t m_socket;
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

		/** recv, recvfrom or accept will not block if this returns true.
			Waits until input is pending, or until the timeout.
		@param[in] ns_timeout:
			The timeout, in nanoseconds.
		@return
			Whether there was some pending input within the timeout. */
		bool pending(
			std::size_t ns_timeout = 0);

		/** Waits for input on at least one of the supplied sockets, or until the timeout.
		@param[in] sockets:
			The sockets to check for.
		@param[out] pending:
			Whether there is input pending on a socket.
			If there is pending input, then recv, recvfrom and accept will not block.
		@param[in] count:
			The amount of sockets supplied.
		@param[in] ns_timeout:
			The timeout, in manooseconds. */
		static void pending(
			Socket const * const * sockets,
			bool * pending,
			std::size_t count,
			std::size_t ns_timeout = 0);

		/** Closes the socket. */
		void close();
		/** Shuts down the socket.
		@param[in] what:
			What aspect to shutdown.
		@return
			Whether it succeeded. */
		bool shutdown(
			Shutdown what);

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
		/** Creates a datagram socket for the requested address family.
			Accepts IPv4 and IPv6. */
		explicit StreamSocket(
			AddressFamily);
		StreamSocket() = default;
		StreamSocket(StreamSocket const&) = delete;
		StreamSocket(StreamSocket &&) = default;

		StreamSocket &operator=(StreamSocket const&) = delete;
		StreamSocket &operator=(StreamSocket &&) = default;

		/** Sends at most `size` bytes of `data`.
		@param[in] data:
			The data to send.
		@param[in] size:
			How many bytes to send at most.
		@return
			The number of bytes sent, or 0 on error. */
		std::size_t send(
			void const* data,
			std::size_t size);
		/** Receives at most `size` bytes into `data`.
		@param[out] data:
			Where to receive the incoming data into.
		@param[in] size:
			How many bytes to receive at most.
		@return
			The number of bytes received, or 0 on error. */
		std::size_t recv(
			void * data,
			std::size_t size);

		/** Tries to connect to `address`.
		@param[in] address:
			The address to connect to.
		@return
			Whether a connection was established. */
		bool connect(
			SocketAddress const& address);
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
		/** Listens on the currently bound address.
		@return
			Whether it succeeded. */
		bool listen();
		/** Accepts an incoming connection.
			This function does not block if `pending()` is `true`.
		@param[out] out:
			The socket to hold the incoming connection.
		@return
			Whether it succeeded. */
		bool accept(
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

		/** Sends at most `size` bytes of `data` to the given address.
		@param[in] data:
			The data to send.
		@param[in] size:
			How many bytes to send.
		@param[in] to:
			The address to send the data to.
		@param[in] flags:
			Constant out of a socket api header. Since the set of options is not uniformly implemented across platforms, implementation-specific values have to be put manually.
		@return
			The number of bytes received, or 0 on error. */
		std::size_t sendto(
			void const * data,
			std::size_t size,
			SocketAddress const& to,
			std::size_t flags = 0);

		/** Receives at most `size` bytes into `data` from the given address.
		@param[in] data:
			Where to receive the incoming data into.
		@param[in] size:
			How many bytes to receive at most.
		@param[in] to:
			The address to send the data to.
		@param[in] flags:
			Constant out of a socket api header. Since the set of options is not uniformly implemented across platforms, implementation-specific values have to be put manually.
		@return
			The number of bytes received, or 0 on error. */
		std::size_t recvfrom(
			void * data,
			std::size_t size,
			SocketAddress const& from,
			std::size_t flags = 0);
	};
}

#include "Socket.inl"

#endif