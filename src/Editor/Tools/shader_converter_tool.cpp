#include <Editor/Tools/shader_converter_tool.h>

#include <imgui.h>

#include <Utility/log.h>
#include <Utility/json_utility.h>
#include <Editor/Tools/resource_accessor_tool.h>

namespace poke::editor {

ShaderConverterTool::ShaderConverterTool(Editor& editor, const bool defaultActive) :
    Tool(defaultActive) 
{
	isActive_ = defaultActive;
	name_ = "Shader Converter tool";
	toolFlag_ = TOOL_FLAG_SHADER_CONVERTER;
}

void ShaderConverterTool::OnDrawImgui() {
	ImGui::Begin(name_.c_str(), &isActive_);

	const Resource outResource = ResourceAccessorTool::DisplayGetResource(
		"Select shader...", ResourceAccessorTool::DisplayType::BUTTON, 
		FileType::CUSTOM, 
		FolderType::RESOURCE);

	if (ResourceAccessorTool::IsResourceValid(outResource)) {
		selectedShader_ = outResource;
	}
	if (ResourceAccessorTool::IsResourceValid(selectedShader_)) {
		ImGui::Text(("Selected shader : " + selectedShader_.name).c_str());
        if(ImGui::Button("Convert to shader")) {
			ConvertToShader(selectedShader_.name);
        }
	}

	ImGui::End();
}

void ShaderConverterTool::OnSetActive(){}

void ShaderConverterTool::OnSetInactive(){}

void ShaderConverterTool::ConvertToShader(std::string& fileName) {
	// Read vert file
	json result = json();
    if(fileName.find('.') != std::string::npos) {
		fileName = fileName.substr(0, fileName.size() - 5);
    }

	result["vert"] = fileName + ".vert";
	result["frag"] = fileName + ".frag";
	//result["comp"] = fileName.substr(0, fileName.size() - 5) + ".comp";

	std::string fileVertContent = PokFileSystem::ReadFile(
		PokFileSystem::GetPath(FileType::SHADER, FolderType::RESOURCE)
		+ result["vert"].get<std::string>(),
		FileType::CUSTOM,
		FolderType::RESOURCE
	);

	std::string fileFragContent = PokFileSystem::ReadFile(
		PokFileSystem::GetPath(FileType::SHADER, FolderType::RESOURCE)
		+ result["frag"].get<std::string>(),
		FileType::CUSTOM,	
		FolderType::RESOURCE
	);

	/*std::string fileCompContent = PokFileSystem::ReadFile(
		PokFileSystem::GetPath(FileType::SHADER, FolderType::ROM) + result["comp"].get<std::string>(),
		FileType::CUSTOM,	
		FolderType::ROM
	);*/

	AnalyseFile(fileVertContent, result);
	AnalyseFile(fileFragContent, result);
	//AnalyseFile(fileCompContent, result);

	// Write Json
	PokFileSystem::WriteFile(
		fileName, 
		result, 
		FileType::COMPILED_SHADER
	);

	LogDebug(
		fileName + 
		    PokFileSystem::GetExtension(FileType::COMPILED_SHADER) + 
		    " created in folder " + 
		    PokFileSystem::GetPath(FileType::COMPILED_SHADER, FolderType::SAVE), 
		LogType::EDITOR_LOG
	);
}

bool ShaderConverterTool::ReadLine(std::string& fileContent, std::string& result) {
	const size_t kNextCarriageReturnPos = fileContent.find_first_of('\n') + 1;
	result = fileContent.substr(0, kNextCarriageReturnPos);

	bool returnValue = !result.empty();
	// Remove \r\n from the extracted line
	if (result.find("\r\n") < result.size()) {	
		result = result.substr(0, result.find("\r\n"));
	}

	// Remove extra whitespaces
	result = Trim(result);

	fileContent = fileContent.substr(
		kNextCarriageReturnPos, 
		fileContent.size() - kNextCarriageReturnPos
	);
	// If there was a line but an empty one, read another one
	if (returnValue && result.empty()) {	
		return ReadLine(fileContent, result);
	}
	return returnValue;
}

std::string ShaderConverterTool::Trim(std::string& str) {
	// Remove tabulations
	while (str.find('\t') <= 1)
		str = str.substr(1, str.size() - 1);

	// Remove spaces
	size_t first = str.find_first_not_of(' ');
	size_t last = str.find_last_not_of(' ');

	if (first < str.size() && last < str.size()) {	
		return str.substr(first, (last - first + 1));
	}
	
	return str;
}

bool ShaderConverterTool::CheckEndOfLine(std::string& line) {
	line = Trim(line);
	return line.empty();
}

std::string ShaderConverterTool::GetInfo(
	std::string& line, 
	const char* searchStr, 
	std::string& fileContent, 
	bool forceNewLine
) {
	size_t pos = line.find_first_of(searchStr);

	// Getting info
	std::string result = line.substr(0, pos);
	pos += std::string(searchStr).size();

	// Removing the rest from the line
	line = line.substr(pos, line.size() - pos);

	if (forceNewLine && CheckEndOfLine(line)) {
		ReadLine(fileContent, line);
	}

	return result;
}

void ShaderConverterTool::AnalyseFile(
	std::string& fileContent, 
	json& result
) {
	// Find attributes
	std::string line;
	int aIndexUniform = CheckJsonParameter(
		result, 
		"uniforms", 
		nlohmann::detail::value_t::array
	) ? result["uniforms"].size() : 0;

	int aIndexUniformBlocks = CheckJsonParameter(
		result, 
		"uniforms_blocks", 
		nlohmann::detail::value_t::array
	) ? result["uniforms_blocks"].size() : 0;

	int aIndexAttribute = CheckJsonParameter(
		result, 
		"attributes", 
		nlohmann::detail::value_t::array
	) ? result["attributes"].size() : 0;

	unsigned bindingAttributeSize = 0;
	while (ReadLine(fileContent, line)) {

		// Getting layout
		if (line.find("layout(") < line.size()) {
			// Removing layout from line
			GetInfo(line, "layout(", fileContent);

			// Getting type (Binding or location)
			std::string type = GetInfo(line, " = ", fileContent);
			int numType = atoi(GetInfo(line, ")", fileContent).c_str());
			std::string uniformType = GetInfo(line, " ", fileContent);

            // We don't take the out parameters
			if (uniformType == "out")
				continue;

			std::string globalDataType = "prout";
			int globalDataIndex = 0;

			//If it's uniform block
			if (line.find(';') > line.size()) {

				std::string dataName = line;

				//Check for doubles
				bool doubleResult = false;
				for (int i = aIndexUniformBlocks - 1; i >= 0; i--) {
					if (CheckJsonExists(result["uniforms_blocks"][i], "name")){
						if (result["uniforms_blocks"][i]["name"]
							.get<std::string>() == dataName) {
							doubleResult = true;
							break;
						}
					}
				}
				if (doubleResult) continue;

				json aUniforms = json::array();
				
				int index = 0;
				unsigned uniformBlockSize = 0;
				ReadLine(fileContent, line);
				while (line.find('}') >= line.size()) {
					//Ignore first line if space is made
					if (line.find('{') <= line.size()) {
						ReadLine(fileContent, line);
						continue;
					}

					std::string type = GetInfo(line, " ", fileContent);
					aUniforms[index]["name"] = GetInfo(line, ";", fileContent);
					aUniforms[index]["type"] = 0; // Because it's like this
					aUniforms[index]["binding"] = -1;
					aUniforms[index]["offset"] = uniformBlockSize; 
					aUniforms[index]["size"] = GetSizeOfType(type);
					aUniforms[index]["glType"] = GetGlType(type);
					aUniforms[index]["readOnly"] = false;
					aUniforms[index]["writeOnly"] = false;
					aUniforms[index]["stageFlags"] = -1; // TODO (@Duncan): To define

					uniformBlockSize += GetSizeOfType(type);
					index++;
				}

				result["uniforms_blocks"][aIndexUniformBlocks]["name"] 
			    = dataName;
				result["uniforms_blocks"][aIndexUniformBlocks]["stageFlags"] 
			    = -1; // TODO (@Duncan): To define
				result["uniforms_blocks"][aIndexUniformBlocks]["size"] 
			    = uniformBlockSize;
				result["uniforms_blocks"][aIndexUniformBlocks]["uniforms"] 
			    = aUniforms;

				aIndexUniformBlocks++;
			} else {
				bool isAttribute = type == "location";
				globalDataType = isAttribute ? "attributes" : "uniform";
				globalDataIndex = isAttribute ? aIndexAttribute : aIndexUniform;

				std::string dataType = GetInfo(line, " ", fileContent);
				std::string dataName = GetInfo(line, ";", fileContent, false);

				//Check for doubles
				bool doubleResult = false;
				for (int i = globalDataIndex - 1; i >= 0; i--) {
                    if (CheckJsonExists(result[globalDataType][i], "name")) {
					    if (result[globalDataType][i]["name"].get<std::string>() 
							== dataName) {
						    doubleResult = true;
					        break;
					    }
                    }
				}
				if (doubleResult) 
					continue;

				//Affecting data into json into Json
				result[globalDataType][globalDataIndex][type] = numType; //Affecting type + num
				result[globalDataType][globalDataIndex]["name"] = dataName; //Affecting name

				result[globalDataType][globalDataIndex]["size"] 
			    = GetSizeOfType(dataType);
				result[globalDataType][globalDataIndex]["glType"] 
			    = GetGlType(dataType);

				if (!isAttribute) {
					result[globalDataType][globalDataIndex]["readOnly"] 
				    = false;
					result[globalDataType][globalDataIndex]["writeOnly"] 
				    = false;
					result[globalDataType][globalDataIndex]["offset"] 
				    = bindingAttributeSize;
					result[globalDataType][globalDataIndex]["stageFlags"] 
				    = -1; // TODO (@Duncan) TO DEFINE

					bindingAttributeSize += GetSizeOfType(dataType);
					aIndexUniform++;
				} else {
					result[globalDataType][globalDataIndex]["set"] = kDefaultSet;
					aIndexAttribute++;
				}
			}
		}
	}
}

int ShaderConverterTool::GetStageFlags(const std::string& value) {

	if (value == ".vert") return 1;
	if (value == ".tesc") return 2;
	if (value == ".tese") return 4;
	if (value == ".geom") return 8;
	if (value == ".frag") return 16;
	if (value == ".comp") return 32;

	return -1;
}

int ShaderConverterTool::GetSizeOfType(const std::string& value) {
	if (value == "float") return 4;
	if (value == "vec2") return 8;
	if (value == "vec3") return 12;
	if (value == "vec4") return 16;
	if (value == "mat4") return 64;

	return -1;
}

int ShaderConverterTool::GetGlType(const std::string& value) {
	if (value == "float") return 5126;
	if (value == "vec2") return 35664;
	if (value == "vec3") return 35665;
	if (value == "vec4") return 35666;
	if (value == "mat4") return 35676;
	
	if (value == "sampler2D") return 35678;
	if (value == "samplerCube") return 35680;
	if (value == "image2D") return 36941;
	if (value == "imageCube") return 36944;
	if (value == "sampler2DMultisample") return 37128;
	if (value == "image2DMultisample") return 36949;

	return -1;
}
} //namespace poke::editor
