#include <Utility/json_utility.h>

#include <string>

namespace poke {
bool IsJsonValueNumeric(const json::value_type & jsonValue) {
	return jsonValue.type() == json::value_t::number_float ||
		   jsonValue.type() == json::value_t::number_integer ||
		   jsonValue.type() == json::value_t::number_unsigned;
}

bool CheckJsonExists(const json& jsonObject, const std::string& parameterName) {
	return jsonObject.find(parameterName) != jsonObject.end();
}

bool CheckJsonExists(const json & jsonObject, const char * parameterName) {
	return jsonObject.find(parameterName) != jsonObject.end();
}

bool CheckJsonParameter(const json& jsonObject, const std::string& parameterName, json::value_t expectedType) {
	return CheckJsonExists(jsonObject, parameterName) && jsonObject[parameterName].type() == expectedType;
}

bool CheckJsonParameter(const json & jsonObject, const char * parameterName, json::value_t expectedType) {
	return CheckJsonExists(jsonObject, parameterName) && jsonObject[parameterName].type() == expectedType;
}

bool CheckJsonNumber(const json& jsonObject, const std::string& parameterName) {
	return CheckJsonParameter(jsonObject, parameterName, json::value_t::number_float) ||
		   CheckJsonParameter(jsonObject, parameterName, json::value_t::number_integer) ||
		   CheckJsonParameter(jsonObject, parameterName, json::value_t::number_unsigned);
}

bool CheckJsonNumber(const json & jsonObject, const char * parameterName)
{
	return CheckJsonParameter(jsonObject, parameterName, json::value_t::number_float) ||
		CheckJsonParameter(jsonObject, parameterName, json::value_t::number_integer) ||
		CheckJsonParameter(jsonObject, parameterName, json::value_t::number_unsigned);
}

json LoadJson(const std::string& jsonName, const FileType fileType, const FolderType folderType) {
	return PokFileSystem::ReadJsonFile(jsonName, fileType, folderType);
}
json LoadJson(const char * jsonName, const FileType fileType, const FolderType folderType)
{
	return PokFileSystem::ReadJsonFile(jsonName, fileType, folderType);
}
}// namespace poke
