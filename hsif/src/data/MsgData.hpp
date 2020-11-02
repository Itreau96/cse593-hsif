#pragma once

#include <any>
#include <string>
#include <memory>

struct MsgData
{
public:
	MsgData(std::any data, size_t length, std::string to, std::string from);
	void bind(std::any data, size_t length, std::string to, std::string from);
	std::string serialize();
private:
	std::string to_;
	std::string from_;
	std::any data_;
	size_t length_;
};

using MsgDataPtr = std::shared_ptr<MsgData>;