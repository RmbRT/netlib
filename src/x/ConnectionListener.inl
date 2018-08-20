#include <utility>

namespace netlib
{
	namespace x
	{
		constexpr ConnectionListener * ConnectionListener::cast_from_base(
			Socket * base)
		{
			return static_cast<ConnectionListener *>(base);
		}

		constexpr ConnectionListener const * ConnectionListener::cast_from_base(
			Socket const * base)
		{
			return static_cast<ConnectionListener const *>(base);
		}

		bool ConnectionListener::listening() const
		{
			return m_listening;
		}
	}
}