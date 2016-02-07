#ifndef __netlib_socket_hpp_defined
#define __netlib_socket_hpp_defined

#include "Defines.hpp"
#include "Protocol.hpp"
#include "SocketAddress.hpp"

namespace netlib
{

	class Socket;

	enum class SocketType: char
	{
		Stream = 1,
		Datagram = 2,
		Raw = 3,
		ReliablyDeliveredMessage = 4,
		SequencedPacketStream = 5
	};
	
	enum class Shutdown
	{
		Receive, Send, Both
	};

	/* Basic Socket class. Represents a generic Socket and contains functions that all types of Sockets have. */
	class Socket
	{
	protected:
		SocketAddress m_address;
		Protocol m_protocol;
		SocketType m_type;
		std::uintptr_t m_socket;
	public:
		Socket();
		Socket(AddressFamily family, SocketType type, Protocol protocol = Protocol::Default);
		Socket(SocketAddress const& address, SocketType type, Protocol protocol = Protocol::Default);
		~Socket();

		/* Socket does not support copying. */
		Socket(Socket const&) = delete;
		Socket&operator=(Socket const&) = delete;

		/* Sockets can only be moved. */
		Socket(Socket &&);
		Socket &operator=(Socket &&);


		/*recv, recvfrom or accept will not block if this returns true.*/
		bool pending();
		void close();
		int shutdown(Shutdown);

		/* Whether this Socket exists. */
		NETLIB_INL bool exists() const;
		/* Returns the address of this Socket. */
		NETLIB_INL SocketAddress const& address() const;
		/* Returns the protocol of this Socket. */
		NETLIB_INL Protocol protocol() const;
		/* Returns the type of this Socket. */
		NETLIB_INL SocketType type() const;
	};

	
	class StreamSocket : public Socket
	{
	public:
		/*Accepts IPv4 and IPv6.*/
		StreamSocket(AddressFamily);
		StreamSocket(){}
		StreamSocket(StreamSocket const& socket) = delete;
		StreamSocket(StreamSocket &&move) : Socket((Socket&&)move) { }


		StreamSocket &operator=(StreamSocket const&) = delete;
		StreamSocket &operator=(StreamSocket &&move);

		size_t send(void const* data, size_t size);
		size_t recv(void * data, size_t size);

		bool connect(SocketAddress const& addr);
		bool bind(SocketAddress const& address);
		bool listen();
		bool accept(StreamSocket &out);
	};

	class DatagramSocket : public Socket
	{
	public:
		DatagramSocket(AddressFamily);
		DatagramSocket &operator=(DatagramSocket &&);
		DatagramSocket(DatagramSocket &&);

		DatagramSocket &operator=(DatagramSocket const&) = delete;
		DatagramSocket(DatagramSocket const&) = delete;

		size_t sendto(void const * data, size_t size, SocketAddress const& to, size_t flags);
		size_t recvfrom(void * data, size_t size, SocketAddress const& from, size_t flags);
	};
}

#include "Socket.inl"

#endif