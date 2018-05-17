#ifndef ARG_STREAM_H
#define ARG_STREAM_H

#include <stdint.h>
#include <string>
#include <vector>
#include "property.h"

class arg_stream
{
public:
	arg_stream() { buffer.reserve(128); }

	int length() { return buffer.size()-rpos; }
	char *get_buffer() { return buffer.data()+rpos; }
	void clear() { buffer.clear(); }

	int8_t get_8() { return *((int8_t*)(buffer.data()+add_rpos(1))); }
	int16_t get_16() { return *((int16_t*)(buffer.data()+add_rpos(2))); }
	int32_t get_32() { return *((int32_t*)(buffer.data()+add_rpos(4))); }
	int64_t get_64() { return *((int64_t*)(buffer.data()+add_rpos(8))); }

	uint8_t get_u8() { return *((uint8_t*)(buffer.data()+add_rpos(1))); }
	uint16_t get_u16() { return *((uint16_t*)(buffer.data()+add_rpos(2))); }
	uint32_t get_u32() { return *((uint32_t*)(buffer.data()+add_rpos(4))); }
	uint64_t get_u64() { return *((uint64_t*)(buffer.data()+add_rpos(8))); }

	float get_float() { return *((float*)(buffer.data()+add_rpos(4))); }
	double get_double() { return *((double*)(buffer.data()+add_rpos(8))); }

	std::string get_string()
	{
		std::string str = "";
		char *data = buffer.data();
		char *end = data+length();
		while(data != end && *data != 0)
		{
			str += *data;
			data++;
		}
		return str;
	}

	std::vector<char> get_blob()
	{
		uint16_t len = 0;
		len = get_u16();
		return std::vector<char>(get_buffer(), get_buffer()+len);
	}

	property get_property()
	{
		ARG_TYPE prop_type = get_u8();

		// UINT
		if (prop_type == ARG_UINT8)
			return property(get_u8());
		else if (prop_type == ARG_UINT16)
			return property(get_u16());
		else if (prop_type == ARG_UINT32)
			return property(get_u32());
		else if (prop_type == ARG_UINT64)
			return property(get_u64());
		// INT
		else if (prop_type == ARG_INT8)
			return property(get_8());
		else if (prop_type == ARG_INT16)
			return property(get_16());
		else if (prop_type == ARG_INT32)
			return property(get_32());
		else if (prop_type == ARG_INT64)
			return property(get_64());
		// FLOAT/DOUBLE
		else if (prop_type == ARG_FLOAT)
			return property(get_float());
		else if (prop_type == ARG_DOUBLE)
			return property(get_double());
		// NON-NUMERIC
		else if (prop_type == ARG_STRING)
			return property(get_string());
		else if (prop_type == ARG_BLOB)
			return property(get_blob());
		else if (prop_type == ARG_ARRAY)
			return property(get_array());
		else if (prop_type == ARG_DICT)
			return property(get_dict());
	}

	std::vector<property> get_array()
	{
		uint16_t len = get_u16();
		std::vector<property> vec;
		vec.reserve(len);
		while (len-- > 0)
			vec.push_back(get_property());
		return vec;
	}

	std::map<property, property> get_dict()
	{
		uint16_t len = get_u16();
		std::map<property, property> dict;
		while (len-- > 0)
		{
			property key = get_property();
			property val = get_property();
			dict[key] = val;
		}
		return dict;
	}

	void put_8(int8_t n) { put_data((char*)&n, sizeof(int8_t)); }
	void put_16(int16_t n) { put_data((char*)&n, sizeof(int16_t)); }
	void put_32(int32_t n) { put_data((char*)&n, sizeof(int32_t)); }
	void put_64(int64_t n) { put_data((char*)&n, sizeof(int64_t)); }

	void put_u8(uint8_t n) { put_data((char*)&n, sizeof(uint8_t)); }
	void put_u16(uint16_t n) { put_data((char*)&n, sizeof(uint16_t)); }
	void put_u32(uint32_t n) { put_data((char*)&n, sizeof(uint32_t)); }
	void put_u64(uint64_t n) { put_data((char*)&n, sizeof(uint64_t)); }

	void put_float(float n) { put_data((char*)&n, sizeof(float)); }
	void put_double(double n) { put_data((char*)&n, sizeof(double)); }

	void put_string(std::string str) { put_data((char*)str.c_str(), str.length()+1/* +1 for \0 */); }

	void put_blob(std::vector<char> blob) { put_u16(blob.size()); put_data(blob); }
	void put_blob(char* data, uint16_t len) { put_u16(len); put_data(data, len); }

	void put_property(property p)
	{
		put_u8(p.get_type());

		// UINT
		if (p.get_type() == ARG_UINT8)
			put_u8(p.get_u8());
		else if (p.get_type() == ARG_UINT16)
			put_u16(p.get_u16());
		else if (p.get_type() == ARG_UINT32)
			put_u32(p.get_u32());
		else if (p.get_type() == ARG_UINT64)
			put_u64(p.get_u64());
		// INT
		else if (p.get_type() == ARG_INT8)
			put_8(p.get_8());
		else if (p.get_type() == ARG_INT16)
			put_16(p.get_16());
		else if (p.get_type() == ARG_INT32)
			put_32(p.get_32());
		else if (p.get_type() == ARG_INT64)
			put_64(p.get_64());
		// FLOAT/DOUBLE
		else if (p.get_type() == ARG_FLOAT)
			put_float(p.get_float());
		else if (p.get_type() == ARG_DOUBLE)
			put_double(p.get_double());
		// NON-NUMERIC
		else if (p.get_type() == ARG_STRING)
			put_string(p.get_string());
		else if (p.get_type() == ARG_BLOB)
			put_blob(p.get_blob());
		else if (p.get_type() == ARG_ARRAY)
			put_array(p.get_array());
		else if (p.get_type() == ARG_DICT)
			put_dict(p.get_dict());
	}

	void put_array(std::vector<property> p)
	{
		put_u16(p.size());
		for (auto it=p.begin(); it!=p.end(); i++)
			put_property(*it);
	}

	void put_dict(std::map<property, property> m)
	{
		put_u16(m.size());
		for (auto it=m.begin(); it!=m.end(); i++)
		{
			std::pair<property, property> p = *it;
			put_property(p.first);
			put_property(p.second);
		}
	}

	void put_data(std::vector<char> data) { buffer.insert(buffer.end(), data.begin(), data.end()); }
	void put_data(char* src, size_t len) { buffer.insert(buffer.end(), src, src+len); }

private:
	int add_rpos(int x) { int old = rpos; rpos += x; return old; }
	std::vector<char> buffer;
	int rpos;
};

#endif // ARG_STREAM_H
