#ifndef __netlib_endian_hpp_defined
#define __netlib_endian_hpp_defined


#include "Defines.hpp"
#include <cstdint>
#include <vector>

namespace netlib
{
	namespace serialize
	{
		static std::uint16_t const host_byte_order_16 = 0x0100;
		static std::uint32_t const host_byte_order_32 = 0x03020100;
		static std::uint64_t const host_byte_order_64 = 0x0706050403020100;
		static float const host_byte_order_flt = 3.8204714345426319e-037f;
		static double const host_byte_order_dbl = 7.9499288951273625e-275;

		NETLIB_INL std::uint8_t uint8(std::uint8_t in)
		{
			return in;
		}
		NETLIB_INL std::uint16_t uint16(std::uint16_t in, std::uint16_t byte_order_16)
		{
			if(host_byte_order_16 == byte_order_16)
				return in;
			else
			{
				std::uint16_t out = 0;
				reinterpret_cast<std::uint8_t*>(&out)[std::uint8_t(byte_order_16)] = std::uint8_t(in);
				reinterpret_cast<std::uint8_t*>(&out)[std::uint8_t(byte_order_16>>8)] = std::uint8_t(in>>8);
				return out;
			}
		}
		NETLIB_INL std::uint32_t uint32(std::uint32_t in, std::uint32_t byte_order_32)
		{
			if(host_byte_order_32 == byte_order_32)
				return in;
			else
			{
				std::uint32_t out = 0;
				reinterpret_cast<std::uint8_t*>(&out)[std::uint8_t(byte_order_32)] = std::uint8_t(in);
				reinterpret_cast<std::uint8_t*>(&out)[std::uint8_t(byte_order_32>>8)] = std::uint8_t(in>>8);
				reinterpret_cast<std::uint8_t*>(&out)[std::uint8_t(byte_order_32>>16)] = std::uint8_t(in>>16);
				reinterpret_cast<std::uint8_t*>(&out)[std::uint8_t(byte_order_32>>24)] = std::uint8_t(in>>24);
				return out;
			}
		}

		NETLIB_INL float flt(float in, float byte_order_flt)
		{
			if(host_byte_order_flt == byte_order_flt)
				return in;
			else
			{
				float out = 0.f;
				reinterpret_cast<std::uint8_t*>(&out)[std::uint8_t(reinterpret_cast<std::uint32_t&>(byte_order_flt))] = std::uint8_t(reinterpret_cast<std::uint32_t&>(in));
				reinterpret_cast<std::uint8_t*>(&out)[std::uint8_t(reinterpret_cast<std::uint32_t&>(byte_order_flt)>>8)] = std::uint8_t(reinterpret_cast<std::uint32_t&>(in)>>8);
				reinterpret_cast<std::uint8_t*>(&out)[std::uint8_t(reinterpret_cast<std::uint32_t&>(byte_order_flt)>>16)] = std::uint8_t(reinterpret_cast<std::uint32_t&>(in)>>16);
				reinterpret_cast<std::uint8_t*>(&out)[std::uint8_t(reinterpret_cast<std::uint32_t&>(byte_order_flt)>>24)] = std::uint8_t(reinterpret_cast<std::uint32_t&>(in)>>24);
				return out;
			}
		}
		
		NETLIB_INL std::uint64_t uint64(std::uint64_t in, std::uint64_t byte_order_64)
		{
			if(host_byte_order_64 == byte_order_64)
				return in;
			else
			{
				std::uint64_t out = 0;
				reinterpret_cast<std::uint8_t*>(&out)[std::uint8_t(byte_order_64)] = std::uint8_t(in);
				reinterpret_cast<std::uint8_t*>(&out)[std::uint8_t(byte_order_64>>8)] = std::uint8_t(in>>8);
				reinterpret_cast<std::uint8_t*>(&out)[std::uint8_t(byte_order_64>>16)] = std::uint8_t(in>>16);
				reinterpret_cast<std::uint8_t*>(&out)[std::uint8_t(byte_order_64>>24)] = std::uint8_t(in>>24);
				reinterpret_cast<std::uint8_t*>(&out)[std::uint8_t(byte_order_64>>32)] = std::uint8_t(in>>32);
				reinterpret_cast<std::uint8_t*>(&out)[std::uint8_t(byte_order_64>>40)] = std::uint8_t(in>>40);
				reinterpret_cast<std::uint8_t*>(&out)[std::uint8_t(byte_order_64>>48)] = std::uint8_t(in>>48);
				reinterpret_cast<std::uint8_t*>(&out)[std::uint8_t(byte_order_64>>56)] = std::uint8_t(in>>56);
				return out;
			}
		}

		NETLIB_INL double dbl(double in, double byte_order_dbl)
		{
			if(host_byte_order_dbl == byte_order_dbl)
				return in;
			else
			{
				double out = 0.f;
				reinterpret_cast<std::uint8_t*>(&out)[std::uint8_t(reinterpret_cast<std::uint64_t&>(byte_order_dbl))] = std::uint8_t(reinterpret_cast<std::uint64_t&>(in));
				reinterpret_cast<std::uint8_t*>(&out)[std::uint8_t(reinterpret_cast<std::uint64_t&>(byte_order_dbl)>>8)] = std::uint8_t(reinterpret_cast<std::uint64_t&>(in)>>8);
				reinterpret_cast<std::uint8_t*>(&out)[std::uint8_t(reinterpret_cast<std::uint64_t&>(byte_order_dbl)>>16)] = std::uint8_t(reinterpret_cast<std::uint64_t&>(in)>>16);
				reinterpret_cast<std::uint8_t*>(&out)[std::uint8_t(reinterpret_cast<std::uint64_t&>(byte_order_dbl)>>24)] = std::uint8_t(reinterpret_cast<std::uint64_t&>(in)>>24);
				reinterpret_cast<std::uint8_t*>(&out)[std::uint8_t(reinterpret_cast<std::uint64_t&>(byte_order_dbl)>>32)] = std::uint8_t(reinterpret_cast<std::uint64_t&>(in)>>32);
				reinterpret_cast<std::uint8_t*>(&out)[std::uint8_t(reinterpret_cast<std::uint64_t&>(byte_order_dbl)>>40)] = std::uint8_t(reinterpret_cast<std::uint64_t&>(in)>>40);
				reinterpret_cast<std::uint8_t*>(&out)[std::uint8_t(reinterpret_cast<std::uint64_t&>(byte_order_dbl)>>48)] = std::uint8_t(reinterpret_cast<std::uint64_t&>(in)>>48);
				reinterpret_cast<std::uint8_t*>(&out)[std::uint8_t(reinterpret_cast<std::uint64_t&>(byte_order_dbl)>>56)] = std::uint8_t(reinterpret_cast<std::uint64_t&>(in)>>56);
				return out;
			}
		}


		class Serializer
		{
			std::vector<std::uint8_t> _data;

			std::uint16_t net_byte_order_16;
			std::uint32_t net_byte_order_32;
			std::uint64_t net_byte_order_64;
			float net_order_flt;
			double net_order_dbl;
		public:
			Serializer(std::uint16_t net_byte_order_16, std::uint32_t net_byte_order_32, std::uint64_t net_byte_order_64, float net_order_flt, double net_order_dbl) : net_byte_order_16(net_byte_order_16), net_byte_order_32(net_byte_order_32), net_byte_order_64(net_byte_order_64), net_order_flt(net_order_flt), net_order_dbl(net_order_dbl) { }

			NETLIB_INL std::uint8_t const * data() const { return _data.data(); }
			NETLIB_INL std::size_t size() const { return _data.size(); }

			NETLIB_INL Serializer &operator<<(std::uint8_t v) { return _data.push_back(v), *this; }
			NETLIB_INL Serializer &operator<<(std::uint16_t v)
			{
				_data.resize(_data.size()+2);
				*reinterpret_cast<std::uint16_t*>((&_data.back())-1) = uint16(v, net_byte_order_16);
				return *this;
			}
			NETLIB_INL Serializer &operator<<(std::uint32_t v)
			{
				_data.resize(_data.size()+4);
				*reinterpret_cast<std::uint32_t*>((&_data.back())-3) = uint32(v, net_byte_order_32);
				return *this;
			}
			NETLIB_INL Serializer &operator<<(std::uint64_t v)
			{
				_data.resize(_data.size()+8);
				*reinterpret_cast<std::uint64_t*>((&_data.back())-7) = uint64(v, net_byte_order_64);
				return *this;
			}
			NETLIB_INL Serializer &operator<<(float v)
			{
				_data.resize(_data.size()+4);
				*reinterpret_cast<float*>(&_data.back()-3) = flt(v, net_order_flt);
				return *this;
			}
			NETLIB_INL Serializer &operator<<(double v)
			{
				_data.reserve(_data.size()+8);
				*reinterpret_cast<double*>(&_data.back()-7) = dbl(v, net_order_dbl);
				return *this;
			}
		};

	}
}






#endif