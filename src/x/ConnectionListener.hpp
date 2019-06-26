/** @file ConnectionListener.hpp
	Contains the netlib::x::ConnectionListener class used for listening for incoming connections. */
#ifndef __netlib_x_connectionlistener_hpp_defined
#define __netlib_x_connectionlistener_hpp_defined

#include "../Socket.hpp"
#include "../defines.hpp"

#include <libcr/primitives.hpp>

namespace netlib::x
{
	/** A connection listener object.
		This class extends the behaviour of the netlib::StreamSocket class to comfortably listen for and accept incoming connections. This class works together with the `netlib::x::Connection` class. */
	class ConnectionListener : private StreamSocket
	{
		friend class ::netlib::Poller;
		/** Whether the connection listener is currently listening for connections. */
		bool m_listening;
	public:
		/** Creates an empty connection listener. */
		ConnectionListener();
		/** Creates a connection listener and makes it listen on the requested address.
			This attempts to listen on the given address. Whether the configuration succeeded can be found out by calling `ConnectionListener::listening`.
		@param[in] listen_addr:
			The address to listen on. */
		ConnectionListener(
			SocketAddress const& listen_addr);

		static constexpr ConnectionListener * cast_from_base(
			Socket * base);
		static constexpr ConnectionListener const * cast_from_base(
			Socket const * base);

		ConnectionListener(ConnectionListener &&);
		ConnectionListener &operator=(ConnectionListener &&);
		ConnectionListener(ConnectionListener const&) = delete;
		ConnectionListener &operator=(ConnectionListener const&) = delete;

		/** Whether the listener is listening for incoming connections. */
		NETLIB_INL bool listening() const;

		/** Tries to bind the listen socket to the given address and port.
			If succeeds, the connection listener can receive incoming connections. The result may at later points in time also be retrieved via listening().
		@param[in] listen_addr:
			The port and address that should be bound to.
		@param[in] reuse_address:
			Whether to attempt to reuse the address.
		@return
			Whether the binding succeeded or not. */
		bool listen(
			netlib::SocketAddress const& listen_addr,
			bool reuse_address = false);

		/** Stops listening for incoming connections. */
		void unlisten();


		/** Accepts an incoming connection.
			Prerequesite is that the listener must be listening. */
		COROUTINE(Accept, void)
		CR_STATE(
			(ConnectionListener *) listener,
			(StreamSocket &) out)
		CR_EXTERNAL

		/** Returns the address this connection points to. */
		using netlib::StreamSocket::address;
	};
}

#include "ConnectionListener.inl"

#endif