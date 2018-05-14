#ifndef ARG_STREAM_H
#define ARG_STREAM_H

#include <stdint.h>
#include <string>
#include <vector>

class arg_stream
{
public:
	arg_stream() { buffer.reserve(128); }

	int add_rpos(int x) { int old = rpos; rpos += x; return old; }
	int length() { return buffer.size()-rpos; }

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
		char *data = (char*)buffer.data();
		while(rpos++ < buffer.size() && *data != 0)
		{
			str += *data;
			data++;
		}
		return str;
	}

	std::vector<uint8_t> get_data(size_t len)
	{
		return std::vector<uint8_t>(buffer.begin()+rpos, buffer.begin()+rpos+length());
	}

	void put_data(std::vector<uint8_t> data)
	{
		buffer.insert(buffer.end(), data.begin(), data.end());
	}

	void put_data(uint8_t* data, size_t len)
	{
		put_data(std::vector<uint8_t>(data, data+len));
	}

private:
	std::vector<uint8_t> buffer;
	int rpos;
};

#endif // ARG_STREAM_H
