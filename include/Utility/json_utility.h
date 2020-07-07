// Copyright (c) 2019-2020, POK Family. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//  * Neither the name of POK Family nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
// OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Author : Duncan Bourquard
// Date : 15.11.2019
//----------------------------------------------------------------------------------

#pragma once

#include <string>

#include <json.hpp>

#include <Utility/file_system.h>

using json = nlohmann::json;

namespace poke
{
/**
* \brief Function that checks if the given json value is a number
*/
bool IsJsonValueNumeric(const json::value_type& jsonValue);
/**
* \brief Function that checks if the given parameter exists in the json
*/
bool CheckJsonExists(const json& jsonObject, const std::string& parameterName);
bool CheckJsonExists(const json& jsonObject, const char* parameterName);
/**
* \brief Function that checks if the parameter exists and is of expected type
*/
bool CheckJsonParameter(const json& jsonObject, const std::string& parameterName, json::value_t expectedType);
bool CheckJsonParameter(const json& jsonObject, const char* parameterName, json::value_t expectedType);
/**
* \brief Function that checks if the parameters exists and is a number
*/
bool CheckJsonNumber(const json& jsonObject, const std::string& parameterName);
bool CheckJsonNumber(const json& jsonObject, const char* parameterName);
/**
* \brief Function that loads a Json file and returns a Json object
*/
json LoadJson(const std::string& jsonName, const FileType fileType = FileType::JSON, const FolderType folderType = FolderType::ROM);
json LoadJson(const char* jsonName, const FileType fileType = FileType::JSON, const FolderType folderType = FolderType::ROM);
}