#ifndef PROPERTY_H
#define PROPERTY_H

#include <string>
#include "stdint.h"
#include "fixed_messages.h"

// generic property class that can hold all types:
// int8-64, uint8-64, float/double, blob, vector of properties, map<property, property>
class property
{
public:
	property() : cur_type(ARG_UINT8) { set_u8(0); }

	property(const property& copy) : cur_type(ARG_UINT8)
	{
		if (copy.is_int())
		{
			value.i64 = copy.get_64();
			cur_type = copy.get_type();
		}
		else if (copy.is_uint())
		{
			value.u64 = copy.get_u64();
			cur_type = copy.get_type();
		}
		else if (copy.is_float() || copy.is_double())
		{
			value.d = copy.get_double();
			cur_type = copy.get_type();
		}
		else if (copy.is_string())
		{
			set_string(copy.get_string());
		}
		else if (copy.is_blob())
		{
			set_blob(copy.get_blob());
		}
		else if (copy.is_array())
		{
			set_array(copy.get_array());
		}
		else if(copy.is_dict())
		{
			set_dict(copy.get_dict());
		}
	}

	property(uint8_t v) : cur_type(ARG_UINT8) { set_u8(v); }
	property(uint16_t v) : cur_type(ARG_UINT8) { set_u16(v); }
	property(uint32_t v) : cur_type(ARG_UINT8) { set_u32(v); }
	property(uint64_t v) : cur_type(ARG_UINT8) { set_u64(v); }

	property(int8_t v) : cur_type(ARG_UINT8) { set_8(v); }
	property(int16_t v) : cur_type(ARG_UINT8) { set_16(v); }
	property(int32_t v) : cur_type(ARG_UINT8) { set_32(v); }
	property(int64_t v) : cur_type(ARG_UINT8) { set_64(v); }

	property(float v) : cur_type(ARG_UINT8) { set_float(v); }
	property(double v) : cur_type(ARG_UINT8) { set_double(v); }

	property(std::string v) : cur_type(ARG_UINT8) { set_string(v); }

	property(std::vector<char> v) : cur_type(ARG_UINT8) { set_blob(v); }
	property(char* data, size_t len) : cur_type(ARG_UINT8) { set_blob(data, len); }

	property(std::vector<property> v) : cur_type(ARG_UINT8) { set_array(v); }
	property(std::map<property, property> v) : cur_type(ARG_UINT8) { set_dict(v); }

	~property()
	{
		delete_ptr();
	}

	////////////////////////////////////////////////////////////

	bool operator==(const property& lhs, const property& rhs)
	{
		if (lhs.is_string() && rhs.is_string())
			return lhs.get_string() == rhs.get_string();
		else if(lhs.has_point() || rhs.has_point())
			return lhs.get_double() == rhs.get_double();
		else if(lhs.is_uint() && rhs.is_uint())
			return lhs.get_u64() == rhs.get_u64();
		else if(lhs.is_number() && rhs.is_number())
			return lhs.get_64() == rhs.get_64();
		else if(lhs.is_blob() && rhs.is_blob())
			return lhs.get_blob() == rhs.get_blob();
		else if(lhs.is_array() && rhs.is_array())
			return lhs.get_array() == rhs.get_array();
		else if(lhs.is_dict() && rhs.is_dict())
			return lhs.get_dict() == rhs.get_dict();

		return false;
	}

	bool operator<(const property& lhs, const property& rhs)
	{
		if (lhs.is_string() || rhs.is_string())
			return lhs.get_string() < rhs.get_string();
		else if (lhs.has_point() || rhs.has_point())
			return lhs.get_double() < rhs.get_double();
		else if (lhs.is_uint() && rhs.is_uint())
			return lhs.get_u64() < rhs.get_u64();
		else
			return lhs.get_64() < rhs.get_64();
	}

	bool operator>(const property& lhs, const property& rhs) { return rhs < lhs; }
	bool operator<=(const property& lhs, const property& rhs) { return lhs < rhs || lhs == rhs; }
	bool operator>=(const property& lhs, const property& rhs) { return lhs > rhs || lhs == rhs; }

	property& operator+(const property& lhs, const property& rhs)
	{
		property tmp(lhs);

		if (lhs.is_double() || rhs.is_double())
			tmp.set_double(lhs.get_double() + rhs.get_double());
		else if (lhs.is_float() || rhs.is_float())
			tmp.set_float(lhs.get_float() + rhs.get_float());
		else
			tmp.set_64(lhs.get_64() + rhs.get_64());

		return tmp;
	}

	property& operator-(const property& lhs, const property& rhs)
	{
		if (!lhs.is_number() || !rhs.is_number())
			return lhs;

		property tmp_lhs(lhs);
		property tmp_rhs(rhs);

		if (lhs.is_double() || rhs.is_double())
		{
			tmp_lhs = tmp_lhs.get_double();
			tmp_rhs = -tmp_rhs.get_double();
		}
		else if (lhs.is_float() || rhs.is_float())
		{
			tmp_lhs = tmp_lhs.get_float();
			tmp_rhs = -tmp_rhs.get_float();
		}
		else
		{
			tmp_lhs = tmp_lhs.get_64();
			tmp_rhs = -tmp_rhs.get_64();
		}

		return tmp_lhs + tmp_rhs;
	}

	property& operator+=(const property& rhs) { return *this + rhs; }
	property& operator-=(const property& rhs) { return *this - rhs; }
	property& operator++() { return *this + property(1); }
	property& operator--() { return *this - property(1); }

	////////////////////////////////////////////////////////////

	void delete_ptr()
	{
		if (cur_type == ARG_STRING)
			delete (std::string*)value.ptr;
		else if (cur_type == ARG_BLOB)
			delete (std::vector<char>*)value.ptr;
		else if (cur_type == ARG_ARRAY)
			delete (std::vector<property>*)value.ptr;
		else if (cur_type == ARG_DICT)
			delete (std::map<property, property>*)value.ptr;
	}

	void setup_ptr()
	{
		if (cur_type == ARG_STRING)
			value.ptr = (void*)(new std::string());
		else if (cur_type == ARG_BLOB)
			value.ptr = (void*)(new std::vector<char>());
		else if (cur_type == ARG_ARRAY)
			value.ptr = (void*)(new std::vector<property>());
		else if (cur_type == ARG_DICT)
			value.ptr = (void*)(new std::map<property, property>());
	}

	void set_type(ARG_TYPE new_type)
	{
		if (new_type == cur_type)
			return;

		delete_ptr();

		cur_type = new_type;

		setup_ptr();
	}

	////////////////////////////////////////////////////////////

	void set_float(float f) { set_type(ARG_FLOAT); value.d = (double)f; }
	void set_double(double d) { set_type(ARG_DOUBLE); value.d = d; }

	void set_u8(uint8_t u) { set_type(ARG_UINT8); value.u64 = (uint64_t)u; }
	void set_u16(uint16_t u) { set_type(ARG_UINT16); value.u64 = (uint64_t)u; }
	void set_u32(uint32_t u) { set_type(ARG_UINT32); value.u64 = (uint64_t)u; }
	void set_u64(uint64_t u) { set_type(ARG_UINT64); value.u64 = u; }

	void set_8(int8_t i) { set_type(ARG_INT8); value.i64 = (int64_t)i; }
	void set_16(int16_t i) { set_type(ARG_INT16); value.i64 = (int64_t)i; }
	void set_32(int32_t i) { set_type(ARG_INT32); value.i64 = (int64_t)i; }
	void set_64(int64_t i) { set_type(ARG_INT64); value.i64 = i; }

	void set_string(std::string str) { set_type(ARG_STRING); *get_string_ptr() = str; }
	void set_blob(std::vector<char> blob) { set_type(ARG_BLOB); *get_blob_ptr() = blob; }
	void set_blob(char* data, size_t len) { set_blob(std::vector<char>(data, data+len); }

	void set_array(std::vector<property> vec) { set_type(ARG_ARRAY); *get_vector_ptr() = vec; }

	void set_dict(std::map<property, property> map) { set_type(ARG_DICT); *get_map_ptr() = map; }

	////////////////////////////////////////////////////////////

	int8_t get_u8() { return (uint8_t)get_u64(); }
	int16_t get_u16() { return (uint16_t)get_u64(); }
	int32_t get_u32() { return (uint32_t)get_u64(); }
	uint64_t get_u64()
	{
		if (is_int())
			return (uint64_t)value.i64;
		else if (is_uint())
			return value.u64;
		else if (is_float() || is_double())
			return (uint64_t)value.d;

		return 0;
	}

	int8_t get_8() { return (int8_t)get_64(); }
	int16_t get_16() { return (int16_t)get_64(); }
	int32_t get_32() { return (int32_t)get_64(); }
	int64_t get_64()
	{
		if (is_int())
			return value.i64;
		else if (is_uint())
			return (int64_t)value.u64;
		else if (is_float() || is_double())
			return (int64_t)value.d;

		return 0;
	}

	float get_float()
	{
		return (float)get_double();
	}
	double get_double()
	{
		if (is_int())
			return (double)value.i64;
		else if(is_uint())
			return (double)value.u64;
		else if(is_float() || is_double())
			return value.d;

		return 0.0;
	}

	std::string get_string()
	{
		if (!is_string())
		{
			if (has_point())
				return std::string(get_double());
			else if (is_uint())
				return std::string(get_u64());
			else if (is_int())
				return std::string(get_64());
			else
				return "";
		}
		else
		{
			return *get_string_ptr();
		}
	}

	std::vector<char> get_blob()
	{
		if (!is_blob())
			return std::vector<char>();
		else
			return *get_blob_ptr();
	}

	std::vector<property> get_array()
	{
		if (!is_array())
			return std::vector<property>();
		else
			return *get_vector_ptr();
	}

	std::map<property, property> get_dict()
	{
		if (!is_array())
			return std::vector<property, property>();
		else
			return *get_map_ptr();
	}

	////////////////////////////////////////////////////////////

	bool is_int()
	{
		return cur_type == ARG_INT8
				|| cur_type == ARG_INT16
				|| cur_type == ARG_INT32
				|| cur_type == ARG_INT64;
	}
	bool is_uint()
	{
		return cur_type == ARG_UINT8
				|| cur_type == ARG_UINT16
				|| cur_type == ARG_UINT32
				|| cur_type == ARG_UINT64;
	}
	bool is_float() { return cur_type == ARG_FLOAT; }
	bool is_double() { return cur_type == ARG_DOUBLE; }
	bool has_point() { return cur_type == ARG_FLOAT || cur_type == ARG_DOUBLE; }
	bool is_number() { return is_int() || is_uint() || is_float() || is_double(); }
	bool is_string() { return cur_type == ARG_STRING; }
	bool is_blob() { return cur_type == ARG_BLOB; }
	bool is_array() { return cur_type == ARG_ARRAY; }
	bool is_dict() { return cur_type == ARG_DICT; }
	ARG_TYPE get_type() { return cur_type; }

private:
	union
	{
		int64_t i64;
		uint64_t u64;
		double d;
		void* ptr;
	} value;

	std::string* get_string_ptr() { return (std::string*)value.ptr; }
	std::vector<char>* get_blob_ptr() { return (std::vector<char>*)value.ptr; }
	std::vector<property>* get_vector_ptr() { return (std::vector<property>*)value.ptr; }
	std::map<property, property>* get_map_ptr() { return (std::map<property, property>*)value.ptr; }

	ARG_TYPE cur_type;
};

#endif // PROPERTY_H
