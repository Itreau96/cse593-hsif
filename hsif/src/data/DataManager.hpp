#pragma once

#include <vector>
#include "MsgData.hpp"
#include <functional>

using DataSendCallback = std::function<bool(std::string, MsgDataPtr)>;

class DataManager
{
public:
	DataManager(DataSendCallback callback);
	void receiveData(MsgDataPtr msg);
private:
	std::vector<MsgDataPtr> msgData_;
	DataSendCallback callback_;
};