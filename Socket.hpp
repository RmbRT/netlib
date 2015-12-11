#ifndef __netlib_socket_hpp_defined
#define __netlib_socket_hpp_defined

#include "Defines.hpp"
#include "Protocol.hpp"
#include "SocketAddress.hpp"

namespace netlib
{

	class Socket;

	enum class SocketType: int
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

	/* The native socket api. */
	namespace socketapi
	{
		typedef std::uint16_t socket_t;
		Socket socket(AddressFamily family, SocketType type, Protocol protocol = Protocol::Default);
		Socket accept(Socket const& sock);
		bool pending(socket_t self);
		
	}

	/* Basic Socket class. Represents a generic Socket and contains functions that all types of Sockets have. */
	class Socket
	{
		SocketAddress m_address;
		Protocol m_protocol;
		SocketType m_type;
		std::uintptr_t m_socket;
	public:
		Socket();
		Socket(AddressFamily family, SocketType type, Protocol protocol = Protocol::Default);
		Socket(const SocketAddress &address, SocketType type, Protocol protocol = Protocol::Default);
		~Socket();

		/* Socket does not support copying. */
		Socket(Socket const&) = delete;
		Socket&operator=(Socket const&) = delete;

		/* Sockets can only be moved. */
		Socket(Socket &&);
		Socket &operator=(Socket &&);


		/*recv, recvfrom or accept will not block if this returns true.*/
		bool pending();

		/* Whether this Socket exists. */
		NETLIB_INL bool exists() const;
		/* Returns the address of this Socket. */
		NETLIB_INL SocketAddress const& address() const;
		/* Returns the protocol of this Socket. */
		NETLIB_INL Protocol protocol() const;
		/* Returns the type of this Socket. */
		NETLIB_INL SocketType type() const;

		void close();
		int shutdown(Shutdown);

	};

	
	class StreamSocket : public Socket
	{
	public:
		/*Accepts IPv4 and IPv6.*/
		StreamSocket(AddressFamily);
		StreamSocket(){}
		StreamSocket(StreamSocket const&socket) = delete;
		StreamSocket(StreamSocket &&move);

		StreamSocket &operator=(StreamSocket const&) = delete;
		NETLIB_INL StreamSocket &operator=(StreamSocket &&move);


		using SocketBase::send;
		using SocketBase::connect;
		using SocketBase::bind;
		using SocketBase::listen;
		using SocketBase::accept;
		using SocketBase::recv;
	};

	class DatagramSocket : public SocketBase
	{
	public:
		DatagramSocket(AddressFamily);
		using SocketBase::sendto;
		using SocketBase::close;
		using SocketBase::isOpen;
		using SocketBase::getAddress;
		using SocketBase::getProtocol;
		using SocketBase::getType;
		using SocketBase::recvfrom;
		using SocketBase::pending;
		using SocketBase::operator bool;
	};
}

#endif