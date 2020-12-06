#pragma once

#include <any>
#include <string>
#include <memory>
#include <nlohmann/json.hpp>

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

using json = nlohmann::json;

/// <summary>
/// Abstraction class for JSON message data.
/// </summary>
struct MsgData
{
public:
	/// <summary>
	/// Constructor with routing identifiers and json data.
	/// </summary>
	/// <param name="to">Identifier of endpoint to send to.</param>
	/// <param name="from">Identifier of endpoint sending message.</param>
	/// <param name="data">JSON data object.</param>
	MsgData(std::string to, std::string from, json data);
	
	/// <summary>
	/// Constructor from serialized JSON.
	/// </summary>
	/// <param name="serialized">Serialized JSON string containing message attributes.</param>
	MsgData(json serialized);

	/// <summary>
	/// Retrieves attributes in properly formatted JSON string form.
	/// </summary>
	/// <returns></returns>
	std::string getJson();

	/// <summary>
	/// Returns whether or not message data is valid.
	/// </summary>
	/// <returns>True if message is properly formatted.</returns>
	bool isValid();
private:
	/// <summary>
	/// Determines if serialized JSON message is valid.
	/// </summary>
	/// <param name="data">Message in JSON form</param>
	/// <returns>Returns whether message has proper identifiers.</returns>
	bool valid(json data);
	// JSON data instance
	json dataJson_;
	// Determines message validity
	bool valid_;
};

using MsgDataPtr = std::shared_ptr<MsgData>;