#include "message_reader.h"
#include "fixed_messages.h"

message_reader::message_reader(message_handler* handler_ptr, tcp_connection* tcp_connection_ptr)
	: handler_ptr(handler_ptr),
	  parent_tcp_connection_ptr(tcp_connection_ptr),
	  rpos(0),
	  wpos(0),
	  cur_msg_id(0)
{
}

/*------------------*/
// bufer operations //
/*------------------*/

bool message_reader::write_to_buffer(char* data, size_t len)
{
	if (len > get_buffer_write_left())
		return false;

	// reading/writing buffer wraps around
	size_t real_wpos = wpos%buffer_capacity;
	size_t real_endpos = (wpos+len)%buffer_capacity;
	if (real_endpos < real_wpos)
	{
		int firstwrite_len = buffer_capacity-real_wpos;
		std::copy(data, data+firstwrite_len, buffer+real_wpos);
		std::copy(data+firstwrite_len, data+len, buffer+0);
	}
	else
	{
		std::copy(data, data+len, buffer+real_wpos);
	}

	wpos += len;
	return true;
}

bool message_reader::read_from_buffer(char* dest, size_t len)
{
	if (len > get_buffer_read_left())
		return false;

	// reading/writing buffer wraps around
	size_t real_rpos = rpos%buffer_capacity;
	size_t real_endpos = (rpos+len)%buffer_capacity;
	if (real_endpos < real_rpos)
	{
		std::copy(buffer+real_rpos, buffer+buffer_capacity, dest);
		std::copy(buffer+0, buffer+real_endpos, dest);
	}
	else
	{
		std::copy(buffer+real_rpos, buffer+real_rpos+len, dest);
	}

	rpos += len;

	// normalize after read to prevent int overflow
	rpos %= buffer_capacity;
	wpos %= buffer_capacity;
	return true;
}

int message_reader::find_in_buffer(char c)
{
	for (int pos=rpos; pos<wpos; pos++)
	{
		int real_pos = pos%buffer_capacity;
		if (*(buffer+real_pos) == c)
			return pos;
	}
	return -1;
}

/*--------------*/
// args reading //
/*--------------*/

message_reader::READ_RESULT message_reader::read_type_to_args_stream(ARG_TYPE next_type)
{
	// ARG_UINT*
	if (next_type == ARG_UINT8)
	{
		if (!buffer_can_read(sizeof(uint8_t)))
			return NEED_READ_MORE;
		uint8_t x = 0;
		read_from_buffer((char*)&x, sizeof(uint8_t));
		cur_arg_stream.put_u8(x);
	}
	else if (next_type == ARG_UINT16)
	{
		if (!buffer_can_read(sizeof(uint16_t)))
			return NEED_READ_MORE;
		uint16_t x = 0;
		read_from_buffer((char*)&x, sizeof(uint16_t));
		cur_arg_stream.put_u16(x);
	}
	else if (next_type == ARG_UINT32)
	{
		if (!buffer_can_read(sizeof(uint32_t)))
			return NEED_READ_MORE;
		uint32_t x = 0;
		read_from_buffer((char*)&x, sizeof(uint32_t));
		cur_arg_stream.put_u32(x);
	}
	else if (next_type == ARG_UINT64)
	{
		if (!buffer_can_read(sizeof(uint64_t)))
			return NEED_READ_MORE;
		uint64_t x = 0;
		read_from_buffer((char*)&x, sizeof(uint64_t));
		cur_arg_stream.put_u64(x);
	}
	// ARG_INT*
	else if (next_type == ARG_INT8)
	{
		if (!buffer_can_read(sizeof(int8_t)))
			return NEED_READ_MORE;
		int8_t x = 0;
		read_from_buffer((char*)&x, sizeof(int8_t));
		cur_arg_stream.put_u8(x);
	}
	else if (next_type == ARG_INT16)
	{
		if (!buffer_can_read(sizeof(int16_t)))
			return NEED_READ_MORE;
		int16_t x = 0;
		read_from_buffer((char*)&x, sizeof(int16_t));
		cur_arg_stream.put_u16(x);
	}
	else if (next_type == ARG_INT32)
	{
		if (!buffer_can_read(sizeof(int32_t)))
			return NEED_READ_MORE;
		int32_t x = 0;
		read_from_buffer((char*)&x, sizeof(int32_t));
		cur_arg_stream.put_u32(x);
	}
	else if (next_type == ARG_INT64)
	{
		if (!buffer_can_read(sizeof(int64_t)))
			return NEED_READ_MORE;
		int64_t x = 0;
		read_from_buffer((char*)&x, sizeof(int64_t));
		cur_arg_stream.put_u64(x);
	}
	// ARG_(FLOAT + DOUBLE)
	else if (next_type == ARG_FLOAT)
	{
		if (!buffer_can_read(sizeof(float)))
			return NEED_READ_MORE;
		float x = 0;
		read_from_buffer((char*)&x, sizeof(float));
		cur_arg_stream.put_float(x);
	}
	else if (next_type == ARG_DOUBLE)
	{
		if (!buffer_can_read(sizeof(double)))
			return NEED_READ_MORE;
		double x = 0;
		read_from_buffer((char*)&x, sizeof(double));
		cur_arg_stream.put_double(x);
	}
	// ARG_STRING
	else if (next_type == ARG_STRING)
	{
		int null_char_pos = find_in_buffer(0);
		if (null_char_pos < 0)
			return NEED_READ_MORE;
		size_t str_len = ((size_t)null_char_pos - rpos);

		char str_bytes[str_len+1]; // +1 for null char terminator
		read_from_buffer(str_bytes, str_len+1);
		cur_arg_stream.put_string(str_bytes);
	}
	// ARG_BLOB
	else if (next_type == ARG_BLOB)
	{
		if (!buffer_can_read(sizeof(BLOB_LEN)))
			return NEED_READ_MORE;
		BLOB_LEN len = 0;
		peek_from_buffer((char*)&len, sizeof(BLOB_LEN));

		if (!buffer_can_read(len+sizeof(BLOB_LEN)))
			return NEED_READ_MORE;
		else
			skip_read_buffer(sizeof(BLOB_LEN)); // skip len we peeked

		char data[len];
		read_from_buffer(data, len);
		cur_arg_stream.put_blob(data, len);
	}
	// TYPED PROPERTY
	else if (next_type == ARG_PROP)
	{
		if (!buffer_can_read(sizeof(ARG_TYPE)))
			return NEED_READ_MORE;
		ARG_TYPE prop_type = ARG_UINT8;

		int old_rpos = rpos;
		int old_wpos = wpos;
		read_from_buffer((char*)&prop_type, sizeof(ARG_TYPE));

		cur_arg_stream.put_u8(prop_type);
		if (prop_type == ARG_PROP)
		{
			std::cout << "message_reader::read_type_to_args_stream():"
						 " ERROR, property type cannot be ARG_PROP." << std::endl;
			return ERROR; // prevent infinite loop here
		}
		READ_RESULT ret = read_type_to_args_stream(prop_type);

		if (ret == NEED_READ_MORE)
		{
			cur_arg_stream.unput(sizeof(prop_type));
			rpos = old_rpos;
			wpos = old_wpos;
		}

		return ret;
	}
	// CONTAINERS
	else if ( ARG_IS_ARRAY(next_type) )
	{
		if (!buffer_can_read(sizeof(ARRAY_LEN)))
			return NEED_READ_MORE;
		ARRAY_LEN len = 0;
		read_from_buffer((char*)&len, sizeof(ARRAY_LEN));
		cur_arg_stream.put_u16(len);

		msg_args_stack.push_back(make_array_args_list(GET_ARRAY_TYPE(next_type), len));
		msg_args_index_stack.push_back(0);
		return CONTINUE;
	}
	else if ( ARG_IS_DICT(next_type) )
	{
		if (!buffer_can_read(sizeof(DICT_LEN)))
			return NEED_READ_MORE;
		DICT_LEN len = 0;
		read_from_buffer((char*)&len, sizeof(DICT_LEN));
		cur_arg_stream.put_u16(len);

		msg_args_stack.push_back(make_dict_args_list(GET_DICT_KEY_TYPE(next_type), GET_DICT_VAL_TYPE(next_type), len));
		msg_args_index_stack.push_back(0);

		return CONTINUE;
	}
	else
	{
		std::cout << "message_reader::read_type_to_args_stream(): ERROR"
					 " next_type invalid: " << (int)next_type << std::endl;
		return ERROR;
	}

	return SUCCESS;
}

/*------------------------------------*/
// continue reading message from data //
/*------------------------------------*/

bool message_reader::process(char* data, size_t len)
{
	if (!write_to_buffer(data, len))
		return false;

	// read all available messages, return true inside loop if NEED_READ_MORE
	while (true)
	{
		// read MSG_ID
		if (msg_args_stack.size() == 0)
		{
			if (!buffer_can_read(sizeof(MSG_ID)))
				return true;

			read_from_buffer((char*)&cur_msg_id, sizeof(MSG_ID));
			std::vector<ARG_TYPE>* msg_args = handler_ptr->get_msg_args_by_id(cur_msg_id);
			if (msg_args == NULL)
			{
				std::cout << "message_reader::process(): received invalid message id: " << cur_msg_id << std::endl;
				return false;
			}

			msg_args_stack.push_back(*msg_args);
			msg_args_index_stack.push_back(0);
		}

		bool message_finished_reading = false;
		// read arguments for cur_msg_id
		while (msg_args_stack.size() > 0)
		{
			if (msg_args_index_stack.back() >= msg_args_stack.back().size())
			{
				msg_args_stack.pop_back();
				msg_args_index_stack.pop_back();
				if (msg_args_stack.size() == 0)
				{
					message_finished_reading = true;
					break;
				}
				else
				{
					msg_args_index_stack.back()++;
					continue;
				}
			}

			ARG_TYPE next_type = msg_args_stack.back()[msg_args_index_stack.back()];
			READ_RESULT result = read_type_to_args_stream(next_type);
			if (result == SUCCESS)
				msg_args_index_stack.back()++;
			else if(result == CONTINUE)
				continue;
			else if (result == NEED_READ_MORE)
				return true;
			else// if (result == ERROR)
			{
				std::cout << "message_reader::process(): error reading message, id was: " << cur_msg_id << std::endl;
				return false;
			}
		}

		if (message_finished_reading)
		{
			handler_ptr->call_network_interface(parent_tcp_connection_ptr, cur_msg_id, cur_arg_stream);
			cur_arg_stream.clear();
		}
	}

	return true;
}
