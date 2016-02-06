#include "HostInfo.hpp"
#include "Platform.hpp"
#include <cassert>

namespace netlib
{
	HostInfo::HostInfo(std::string name): m_valid(false)
	{
		::sockaddr_in6 a;
		HostInfo info;
		hostent *hostent = gethostbyname(name.c_str());
		if(!hostent)
			return;

		m_name = hostent->h_name;
		m_address_type = (AddressFamily)hostent->h_addrtype;

		unsigned i = 0;
		while(hostent->h_addr_list[i])
			i++;

		m_addresses.clear();

		switch(m_address_type)
		{
		case AddressFamily::IPv4:
			m_addresses.reserve(i);
			while(i--)
			{
				IPv4Address temp = *reinterpret_cast<const IPv4Address*>(hostent->h_addr_list[i]);
				//temp = ntohl(temp);
				m_addresses.push_back(SocketAddress(IPv4SocketAddress(temp, 0)));
			} break;
		default:
			{
				assert(!"ip v6 addresses are not supported yet.");
			} break;
		}
		i = 0;
		while(hostent->h_aliases[i])
			i++;
		m_aliases.clear();
		m_aliases.reserve(i);
		while(i--)
			m_aliases.push_back(hostent->h_aliases[i]);
			
		m_valid = true;
	}
}