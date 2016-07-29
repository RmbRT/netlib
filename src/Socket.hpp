/** @file Socket.hpp
	Contains the Socket classes and functions.
	This is the 'heart' of the netlib. Main functionality resolving around sockets is all declared in here. */
#ifndef __netlib_socket_hpp_defined
#define __netlib_socket_hpp_defined

#include "defines.hpp"
#include "Protocol.hpp"
#include "SocketAddress.hpp"

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

	/** Converts a socket type to its corresponding native socket api constant. */
	int to_native_api(
		SocketType type);
	
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


		/** recv, recvfrom or accept will not block if this returns true.*/
		bool pending();
		/** Closes the socket. */
		void close();
		/** Shuts down the socket.
		@param[in] what:
			What aspect to shutdown. */
		int shutdown(Shutdown what);

		/** Whether this Socket exists. */
		NETLIB_INL bool exists() const;
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
		/** Accepts IPv4 and IPv6. */
		explicit StreamSocket(
			AddressFamily);
		StreamSocket() = default;
		StreamSocket(StreamSocket const&) = delete;
		StreamSocket(StreamSocket &&) = default;

		StreamSocket &operator=(StreamSocket const&) = delete;
		StreamSocket &operator=(StreamSocket &&) = default;

		std::size_t send(void const* data, std::size_t size);
		std::size_t recv(void * data, std::size_t size);

		bool connect(SocketAddress const& addr);
		bool bind(SocketAddress const& address);
		bool listen();
		bool accept(StreamSocket &out);
	};

	/** Represents a datagram socket. */
	class DatagramSocket : public Socket
	{
	public:
		DatagramSocket(
			AddressFamily);
		DatagramSocket &operator=(DatagramSocket &&) = default;
		DatagramSocket(DatagramSocket &&) = default;

		DatagramSocket &operator=(DatagramSocket const&) = delete;
		DatagramSocket(DatagramSocket const&) = delete;

		std::size_t sendto(
			void const * data,
			std::size_t size,
			SocketAddress const& to,
			std::size_t flags);

		std::size_t recvfrom(
			void * data,
			std::size_t size,
			SocketAddress const& from,
			std::size_t flags);
	};
}

#include "Socket.inl"

#endif