#include "DataManager.hpp"

DataManager::DataManager(DataSendCallback callback) : callback_(callback)
{ }

void DataManager::receiveData(MsgDataPtr msg)
{
	msgData_.push_back(msg);
}