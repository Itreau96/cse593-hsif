#include "MsgData.hpp"
#include "gtest/gtest.h"
#include <nlohmann/json.hpp>
#include <filesystem>

// Test msgData initialization from parameters
TEST(TestMsgData, TestInit)
{
	json testData;
	auto msgData = std::make_shared<MsgData>("to", "from", testData);
	ASSERT_TRUE(msgData != nullptr);
}

// Test msgData initialization from string
TEST(TestMsgData, TestInitFromStr)
{
	json testData;
	testData["to"] = "to";
	testData["from"] = "from";
	testData["data"] = "{ \"test\" : \"test\"}";
	auto msgData = std::make_shared<MsgData>(testData.dump());
	ASSERT_TRUE(msgData != nullptr);
}

// Test msgData validity
TEST(TestMsgData, TestValidConstructor)
{
	json invalidJson;
	auto msgData = MsgData(invalidJson);
	ASSERT_FALSE(msgData.isValid());
}

// Test msgData json return
TEST(TestMsgData, TestGetJson)
{
	json testData;
	testData["to"] = "to";
	testData["from"] = "from";
	testData["data"] = "{ \"test\" : \"test\"}";
	auto msgData = MsgData(testData);
	json returnData = json::parse(msgData.getJson());
	ASSERT_TRUE(testData == returnData);
}