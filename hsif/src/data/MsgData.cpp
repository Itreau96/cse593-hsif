#include "MsgData.hpp"

/*
Copyright 2020 Itreau Bigsby

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

@author   Itreau Bigsby    mailto:ibigsby@asu.edu
 */

MsgData::MsgData(std::string to, std::string from, json data) : valid_(true)
{
	// Set JSON fields from constructor parameters
	dataJson_["type"] = "message";
	dataJson_["to"] = to;
	dataJson_["from"] = from;
	dataJson_["data"] = data;
}

MsgData::MsgData(json serialized) : valid_(true)
{
	// If message is properly formed, set class property
	if (valid(serialized))
	{
		dataJson_ = serialized;
	}
}

bool MsgData::valid(json data)
{
	// Return whether json data has all necessary HSIF message fields
	valid_ = data.contains(std::string("to")) &&
		data.contains(std::string("from")) &&
		data.contains(std::string("data"));
	return valid_;
}

std::string MsgData::getJson()
{
	// Return JSON formatted string
	return dataJson_.dump();
}

bool MsgData::isValid()
{
	return valid_;
}