/* Netlib - netlib.hpp
	this file contains the socket classes that wrap around the platform specific socket libraries.*/

#ifndef __netlib_hpp_defined
#define __netlib_hpp_defined

#include <cstdint>
#include <cstddef>
#include <string>
#include <vector>
#include <cassert>

#include "Defines.hpp"

/*Indicates that IPv6 support was not completed yet.*/
#define NETLIB_IPV6_WIP

namespace netlib
{
	bool startupSocketAPI();
	bool isSocketAPIInitialized();
	void cleanupSocketAPI();

	typedef unsigned char ubyte_t;
		
	enum class AddressFamily : std::int32_t
	{
		Unix = 1,
		Local = Unix,
		IPv4 = 2,
		IPX = 4,
		AppleTalk = 5,
		IPv6 = 10,
		IRDA = 23,
		Bluetooth = 31
	};
	/* IPPROTO_* constants. Some are left out as they were not deemed necessary. */
	enum class Protocol: int
	{
		/*Automatically chooses the default Protocol for a @{SocketType}*/
		Default = 0,
		/*TCP*/
		TransmissionControl = 6, TCP = TransmissionControl,
		/*UDP*/
		UserDatagram = 17, UDP = UserDatagram,
		Raw = 255
	};

	enum class SocketType: int
	{
		Stream = 1,
		Datagram = 2,
		Raw = 3,
		ReliablyDeliveredMessage = 4,
		SequencedPacketStream = 5
	};
		
	PACKED_STRUCT IPv4Address
	{
		IPv4Address(): d0(0), d1(0), d2(0), d3(0) { }
		IPv4Address(ubyte_t d0, ubyte_t d1, ubyte_t d2, ubyte_t d3): d0(d0), d1(d1), d2(d2), d3(d3) { }
		/*format: d0.d1.d2.d3*/
		IPv4Address(const char* data) { IPv4Address::parse(data, *this); }

		/*format: d0.d1.d2.d3*/
		static bool parse(const char* data, IPv4Address &out);
			
		static const IPv4Address Any;

		NETLIB_INL bool operator==(const IPv4Address &other) const { return reinterpret_cast<const int&>(*this) == reinterpret_cast<const int&>(other); }
		NETLIB_INL bool operator!=(const IPv4Address &other) const { return reinterpret_cast<const int&>(*this) != reinterpret_cast<const int&>(other); }
		NETLIB_INL bool operator!() const { return reinterpret_cast<const int&>(*this); }

		ubyte_t d0, d1, d2, d3;
	};

	PACKED_STRUCT IPv6Address
	{
		IPv6Address() : d0(0), d1(0), d2(0), d3(0), d4(0), d5(0), d6(0), d7(0) { }
		IPv6Address(std::uint16_t d0, std::uint16_t d1, std::uint16_t d2, std::uint16_t d3, std::uint16_t d4, std::uint16_t d5, std::uint16_t d6, std::uint16_t d7) : d0(d0), d1(d1), d2(d2), d3(d3), d4(d4), d5(d5), d6(d6), d7(d7) { }
		/*format: d0:d1:d2:d3:d4:d5:d6:d7*/
		IPv6Address(const char* data) : IPv6Address() { IPv6Address::parse(data, *this); }

		/*format: d0:d1:d2:d3:d4:d5:d6:d7*/
		static bool parse(const char* data, IPv6Address &out);

		std::uint16_t d0, d1, d2, d3, d4, d5, d6, d7;
	};

	PACKED_STRUCT IPv6SocketAddress
	{
		std::uint16_t port;
		std::uint32_t field;
		IPv6Address address;
		std::uint32_t scope;
	};

	PACKED_STRUCT IPv4SocketAddress
	{
		IPv4SocketAddress(const IPv4Address &address, std::uint16_t port): port(port), address(address) { }
		/*format: ipv4:port (%u.%u.%u.%u:%u)
		ex.: "127.0.0.1:80"*/
		IPv4SocketAddress(const char* data) { IPv4SocketAddress::parse(data, *this); }
		IPv4SocketAddress(): address(), port() { }

		static bool parse(const char* data, IPv4SocketAddress &out);

		bool operator==(const IPv4SocketAddress &other) const { return (address == other.address) && (port == other.port); }
		bool operator!=(const IPv4SocketAddress &other) const { return (address != other.address) || (port != other.port); }
			
		std::uint16_t port;
		IPv4Address address;
		ubyte_t unused[8];
	};

	PACKED_STRUCT SocketAddress
	{
	private:
		friend class SocketBase;
		SocketAddress(AddressFamily family): family(family), raw({{0x00}}) {}
	public:
		SocketAddress(const IPv4SocketAddress &address) : family(AddressFamily::IPv4), raw(reinterpret_cast<const Address&>(address)) {}
		SocketAddress(const char* str);
		SocketAddress(): family(), raw() { }
		
		static bool parse(const char *str, SocketAddress &out);

		AddressFamily family;
		struct Address{ ubyte_t data[14]; } raw;

		NETLIB_INL IPv4SocketAddress &IPv4() { return *reinterpret_cast<IPv4SocketAddress*>(raw.data); }
		NETLIB_INL const IPv4SocketAddress &IPv4() const { return *reinterpret_cast<const IPv4SocketAddress*>(raw.data); }
	};
	/*Stores the host information. Note that it can either contain IPv4 or IPv6 addresses.
	Before accessing the addresses, the type of address must be checked.*/
	class HostInfo
	{
		std::string _name;
		std::vector<std::string> _aliases;
		AddressFamily _address_type;
		std::vector<IPv4Address> _address_list_v4;
		std::vector<IPv6Address> _address_list_v6;

	public:
		static bool hostInfoByName(const std::string &name, HostInfo &out);

		NETLIB_INL const std::string &name() const { return _name; }
		NETLIB_INL const std::vector<std::string> &aliases() const { return _aliases; }
		NETLIB_INL AddressFamily address_type() const { return _address_type; }
		NETLIB_INL const std::vector<IPv4Address> &ipv4_addresses() const { assert(_address_type == AddressFamily::IPv4); return reinterpret_cast<const std::vector<IPv4Address> &>(_address_list_v4); }
		NETLIB_INL const std::vector<IPv6Address> &ipv6_addresses() const { assert(_address_type == AddressFamily::IPv4); return reinterpret_cast<const std::vector<IPv6Address> &>(_address_list_v6); }

		HostInfo() = default;
		HostInfo(HostInfo&&m) :
			_name(std::move(m._name)), _aliases(std::move(m._aliases)),
			_address_type(m._address_type), _address_list_v4(std::move(m._address_list_v4)), _address_list_v6(std::move(m._address_list_v6)) { }
		HostInfo& operator=(HostInfo&&m) {
			_name = std::move(m._name); _aliases = std::move(m._aliases);
			_address_type = m._address_type; _address_list_v4 = std::move(m._address_list_v4); _address_list_v6 = std::move(m._address_list_v6); }
	};

	enum class Shutdown
	{
		Receive, Send, Both
	};

	class SocketBase
	{
		SocketAddress address;
		Protocol protocol;
		SocketType type;
		std::uintptr_t socket;
		SocketBase(const SocketAddress &addr, Protocol protocol, SocketType type, std::uintptr_t socket);
	public:
		SocketBase(AddressFamily family, SocketType type, Protocol protocol = Protocol::Default);
		SocketBase(const SocketAddress &address, SocketType type, Protocol protocol = Protocol::Default);
		SocketBase();
		~SocketBase() { close(); }

		SocketBase(const SocketBase &) = delete;
		SocketBase &operator=(const SocketBase &) = delete;

		SocketBase(SocketBase &&);
		SocketBase &operator=(SocketBase &&);

		NETLIB_INL operator bool() const { return isOpen(); }

		bool connect(const SocketAddress &serverAddress);

		int send(const void* data, std::size_t size);
		int sendto(const SocketAddress &address, const void *data, std::size_t size);
		int recv(void* data, std::size_t size);
		int recvfrom(const SocketAddress &address, void *data, std::size_t size);

		/*recv, recvfrom or accept will not block if this returns true.*/
		bool pending();

		NETLIB_INL bool isOpen() const { return socket != -1; }
		void close();
		int shutdown(Shutdown);

		bool bind(const SocketAddress &address);

		bool listen();

		bool accept(SocketBase &socket);
	};

	class StreamSocket : private SocketBase
	{
	public:
		/*Accepts IPv4 and IPv6.*/
		StreamSocket(AddressFamily);
		StreamSocket(){}
		StreamSocket(StreamSocket const&socket) = delete;
		StreamSocket(StreamSocket &&move) : SocketBase(std::move(move)) { }

		using SocketBase::send;
		using SocketBase::connect;
		using SocketBase::close;
		using SocketBase::isOpen;
		using SocketBase::bind;
		using SocketBase::listen;
		using SocketBase::accept;
		using SocketBase::pending;
		using SocketBase::recv;
		using SocketBase::operator bool;

		StreamSocket &operator=(StreamSocket const&) = delete;
		NETLIB_INL StreamSocket &operator=(StreamSocket &&move) { ((SocketBase&)*this) = std::move((SocketBase&)move); return *this; }

		NETLIB_INL bool accept(StreamSocket &socket) { return SocketBase::accept(socket); }
	};

	class DatagramSocket : private SocketBase
	{
	public:
		DatagramSocket(AddressFamily);
		using SocketBase::sendto;
		using SocketBase::close;
		using SocketBase::isOpen;
		using SocketBase::recvfrom;
		using SocketBase::pending;
		using SocketBase::operator bool;
	};
}

#endif