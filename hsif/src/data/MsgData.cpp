#include "MsgData.hpp"

MsgData::MsgData(std::any data, size_t length, std::string to, std::string from) :
	data_(data),
	length_(length),
	to_(to),
	from_(from)
{
	
}

void MsgData::bind(std::any data, size_t length, std::string to, std::string from)
{
	data_ = data;
	length_ = length;
	to_ = to;
	from_ = from;
}

std::string MsgData::serialize()
{
	return std::string(""); // Need some simple string serialization of to, from, data, and length attributes
}