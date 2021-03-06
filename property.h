#ifndef PROPERTY_H
#define PROPERTY_H

#include <string>
#include <sstream>
#include <iostream>
#include "stdint.h"
#include "fixed_messages.h"

// generic property class that can hold all types:
// int8-64, uint8-64, float/double, blob, vector<property>, map<property, property>
class property
{
public:
	property() : cur_type(ARG_UINT8) { set_u8(0); }

	property(const property& copy) : cur_type(ARG_UINT8) { set(copy); }

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
	property(const char* v) : cur_type(ARG_UINT8) { set_string(v); }
	property(char v) : cur_type(ARG_UINT8) { std::string s = ""; s += v; set_string(s); }

	property(std::vector<char> v) : cur_type(ARG_UINT8) { set_blob(v); }
	property(char* data, size_t len) : cur_type(ARG_UINT8) { set_blob(data, len); }

	property(std::vector<property> v) : cur_type(ARG_UINT8) { set_array(v); }
	property(std::map<property, property> v) : cur_type(ARG_UINT8) { set_dict(v); }

	~property() { delete_ptr(); }

	/*-----------*/
	/* operators */
	/*-----------*/

	// == and < share:
	#define PROPERTY_CMP_OP(OP)\
		if (is_number() && rhs.is_number())\
		{\
			if(has_point() || rhs.has_point())\
				return get_double() OP rhs.get_double();\
			else if(is_uint() && rhs.is_uint())\
				return get_u64() OP rhs.get_u64();\
			else\
				return get_64() OP rhs.get_64();\
		}\
		else if (is_string() && rhs.is_string())\
			return get_string() OP rhs.get_string();\
		else if(is_blob() && rhs.is_blob())\
			return get_blob() OP rhs.get_blob();\
		else if(is_array() && rhs.is_array())\
			return get_array() OP rhs.get_array();\
		else if(is_dict() && rhs.is_dict())\
			return get_dict() OP rhs.get_dict();

	bool operator==(const property& rhs) const
	{
		PROPERTY_CMP_OP(==);
		return false;
	}

	bool operator<(const property& rhs) const
	{
		PROPERTY_CMP_OP(<);
		// for std::map sorting:
		return get_type() < rhs.get_type();
	}

	// operators +, -, *, and / can share code:
	#define PROPERTY_ARITHMETIC_OP(OP)\
		property tmp(*this);\
\
		if (is_double() || rhs.is_double())\
			tmp.set_double(get_double() OP rhs.get_double());\
		else if (is_float() || rhs.is_float())\
			tmp.set_float(get_float() OP rhs.get_float());\
		else if (get_type() == rhs.get_type())\
		{\
			if (get_type() == ARG_UINT8)\
				tmp.set_u8(get_u8() OP rhs.get_u8());\
			else if (get_type() == ARG_UINT16)\
				tmp.set_u16(get_u16() OP rhs.get_u16());\
			else if (get_type() == ARG_UINT32)\
				tmp.set_u32(get_u32() OP rhs.get_u32());\
			else if (get_type() == ARG_UINT64)\
				tmp.set_u64(get_u64() OP rhs.get_u64());\
			else if (get_type() == ARG_INT8)\
				tmp.set_8(get_8() OP rhs.get_8());\
			else if (get_type() == ARG_INT16)\
				tmp.set_16(get_16() OP rhs.get_16());\
			else if (get_type() == ARG_INT32)\
				tmp.set_32(get_32() OP rhs.get_32());\
			else if (get_type() == ARG_INT64)\
				tmp.set_64(get_64() OP rhs.get_64());\
		}\
		else if (is_uint() && rhs.is_uint())\
			tmp.set_u64(get_u64() OP rhs.get_u64());\
		else if (is_number() && rhs.is_number())\
			tmp.set_64(get_64() OP rhs.get_64());\
\
		return tmp;

	property operator+(const property& rhs) const
	{
		if (is_string() || rhs.is_string())
			return property(get_string() + rhs.get_string());

		PROPERTY_ARITHMETIC_OP(+);
	}

	property operator-(const property& rhs) const { PROPERTY_ARITHMETIC_OP(-); }
	property operator*(const property& rhs) const { PROPERTY_ARITHMETIC_OP(*); }
	property operator/(const property& rhs) const { PROPERTY_ARITHMETIC_OP(/); }

	bool operator!=(const property& rhs) const { return !(*this == rhs); }
	bool operator>(const property& rhs) const { return rhs < *this; }
	bool operator<=(const property& rhs) const { return *this < rhs || *this == rhs; }
	bool operator>=(const property& rhs) const { return *this > rhs || *this == rhs; }
	property& operator=(const property& rhs) { set(rhs); return *this; }
	property& operator*=(const property& rhs) { return *this = *this * rhs; }
	property& operator/=(const property& rhs) { return *this = *this / rhs; }
	property& operator+=(const property& rhs) { return *this = *this + rhs; }
	property& operator-=(const property& rhs) { return *this = *this - rhs; }

	property& operator[](property elem)
	{
		if (is_dict())
			return (*get_map_ptr())[elem];
		else if (is_array())
			return (*get_vector_ptr())[elem.get_u64()];
		else
			return *this;
	}

	property& operator++()
	{
		property one_same_type = 1;
		one_same_type.set_type(get_type());
		return *this = *this + one_same_type;
	}
	property& operator--()
	{
		property one_same_type = 1;
		one_same_type.set_type(get_type());
		return *this = *this - one_same_type;
	}

	/*---------------------*/
	/* handle type changes */
	/*---------------------*/

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

	/*--------------------*/
	/* set property value */
	/*--------------------*/

	void set(const property& copy)
	{
		if (copy.is_int())
		{
			set_64(copy.get_64());
			cur_type = copy.get_type();
		}
		else if (copy.is_uint())
		{
			set_u64(copy.get_u64());
			cur_type = copy.get_type();
		}
		else if (copy.is_float() || copy.is_double())
		{
			set_double(copy.get_double());
			cur_type = copy.get_type();
		}
		// non numeric
		else if (copy.is_string())
			set_string(copy.get_string());
		else if (copy.is_blob())
			set_blob(copy.get_blob());
		else if (copy.is_array())
			set_array(copy.get_array());
		else if(copy.is_dict())
			set_dict(copy.get_dict());
	}

	void set_float(float f) { set_type(ARG_FLOAT); value.d = (double)f; }
	void set_double(double d) { set_type(ARG_DOUBLE); value.d = d; }

	void set_bool(bool b) { set_u8(b); }

	void set_u8(uint8_t u) { set_type(ARG_UINT8); value.u64 = (uint64_t)u; }
	void set_u16(uint16_t u) { set_type(ARG_UINT16); value.u64 = (uint64_t)u; }
	void set_u32(uint32_t u) { set_type(ARG_UINT32); value.u64 = (uint64_t)u; }
	void set_u64(uint64_t u) { set_type(ARG_UINT64); value.u64 = u; }

	void set_8(int8_t i) { set_type(ARG_INT8); value.i64 = (int64_t)i; }
	void set_16(int16_t i) { set_type(ARG_INT16); value.i64 = (int64_t)i; }
	void set_32(int32_t i) { set_type(ARG_INT32); value.i64 = (int64_t)i; }
	void set_64(int64_t i) { set_type(ARG_INT64); value.i64 = i; }

	void set_string(std::string str) { set_type(ARG_STRING); *get_string_ptr() = str; }
	void set_blob(std::vector<char> blob=std::vector<char>()) { set_type(ARG_BLOB); *get_blob_ptr() = blob; }
	void set_blob(char* data, size_t len) { set_blob(std::vector<char>(data, data+len)); }

	void set_array(std::vector<property> vec=std::vector<property>()) { set_type(ARG_ARRAY); *get_vector_ptr() = vec; }

	void set_dict(std::map<property, property> map=std::map<property, property>()) { set_type(ARG_DICT); *get_map_ptr() = map; }

	/*--------------------*/
	/* get property value */
	/*--------------------*/

	#define PROPERTY_RETURN_NUM(type)\
		if (is_int())\
			return (type)value.i64;\
		else if (is_uint())\
			return (type)value.u64;\
		else if (is_float() || is_double())\
			return (type)value.d;\
		else\
			return 0;

	bool get_bool() const { return get_u8()==0; }

	int8_t get_u8() const { return (uint8_t)get_u64(); }
	int16_t get_u16() const { return (uint16_t)get_u64(); }
	int32_t get_u32() const { return (uint32_t)get_u64(); }
	uint64_t get_u64() const { PROPERTY_RETURN_NUM(uint64_t); }

	int8_t get_8() const { return (int8_t)get_64(); }
	int16_t get_16() const { return (int16_t)get_64(); }
	int32_t get_32() const { return (int32_t)get_64(); }
	int64_t get_64() const { PROPERTY_RETURN_NUM(int64_t); }

	float get_float() const { return (float)get_double(); }
	double get_double() const { PROPERTY_RETURN_NUM(double); }

	std::string get_string() const
	{
		if (!is_string())
		{
			std::stringstream s;
			if (is_double())
				s << get_double();
			else if (is_float())
				s << get_float();
			else if (is_uint())
				s << get_u64();
			else if (is_int())
				s << get_64();
			return s.str();
		}
		else
			return *get_string_ptr();
	}

	std::vector<char>& get_blob() const
	{
		if (is_blob())
			return *get_blob_ptr();
		else
			throw std::runtime_error("property::get_blob(): property is not currently a blob!");
	}

	std::vector<property>& get_array() const
	{
		if (is_array())
			return *get_vector_ptr();
		else
			throw std::runtime_error("property::get_array(): property is not currently an array!");
	}

	std::map<property, property>& get_dict() const
	{
		if (is_dict())
			return *get_map_ptr();
		else
			throw std::runtime_error("property::get_dict(): property is not currently a dict!");
	}

	/*------------*/
	/* check type */
	/*------------*/

	bool is_int() const
	{
		return cur_type == ARG_INT8
				|| cur_type == ARG_INT16
				|| cur_type == ARG_INT32
				|| cur_type == ARG_INT64;
	}
	bool is_uint() const
	{
		return cur_type == ARG_UINT8
				|| cur_type == ARG_UINT16
				|| cur_type == ARG_UINT32
				|| cur_type == ARG_UINT64;
	}
	bool is_float() const { return cur_type == ARG_FLOAT; }
	bool is_double() const { return cur_type == ARG_DOUBLE; }
	bool has_point() const { return cur_type == ARG_FLOAT || cur_type == ARG_DOUBLE; }
	bool is_number() const { return is_int() || is_uint() || is_float() || is_double(); }
	bool is_string() const { return cur_type == ARG_STRING; }
	bool is_blob() const { return cur_type == ARG_BLOB; }
	bool is_array() const { return cur_type == ARG_ARRAY; }
	bool is_dict() const { return cur_type == ARG_DICT; }
	ARG_TYPE get_type() const { return cur_type; }

private:
	union
	{
		int64_t i64;
		uint64_t u64;
		double d;
		void* ptr;
	} value;

	std::string* get_string_ptr() const { return (std::string*)value.ptr; }
	std::vector<char>* get_blob_ptr() const { return (std::vector<char>*)value.ptr; }
	std::vector<property>* get_vector_ptr() const { return (std::vector<property>*)value.ptr; }
	std::map<property, property>* get_map_ptr() const { return (std::map<property, property>*)value.ptr; }

	ARG_TYPE cur_type;
};

#endif // PROPERTY_H
