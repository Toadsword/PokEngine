#include <Utility/log.h>

#include <iostream>
#include <string>
#include <thread>
#include <mutex>

#include <Utility/file_system.h>
#include <Utility/time_custom.h>

namespace poke {

Logger & Logger::Get() {
	static Logger instance;
	return instance;
}

void Logger::WriteLog(const std::string& s, const LogType type, const LogSeverity severity) {
	
	std::lock_guard<std::mutex> lock(logMutex_);

	//Preparing and formatting message
	std::string resLog = "";

	resLog += "[";
	resLog += GetLogTypeString(type);
	resLog += "] ";

	switch(severity) {
	    case LogSeverity::DEBUG:
		    resLog += "[DEBUG] ";
		    break;
	    case LogSeverity::WARNING:
		    resLog += "[WARNING] ";
		    break;
	    case LogSeverity::ERROR:
		    resLog += "[ERROR] ";
		    break;
	    default:
		    break;
	}
	
	double timeInMilli = Time::Get().GetTimeMilli();

	const unsigned sec = timeInMilli / 1'000;
	const unsigned min = sec / 60;
	const unsigned hou = min / 60;
	resLog += "[" + std::to_string(hou) + ":" 
				+ std::to_string(min % 60) + ":" + std::to_string(sec % 60) 
				+ "," + std::to_string(static_cast<int>(timeInMilli) % 1'000) 
				+ " | " + std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id())) + " ] ";

	resLog += s;

	// Adding to history
	if (logHistory_.size() <= vectorIndex_)
		logHistory_.resize(logHistory_.size() + kNumPerResize_);
	logHistory_.at(vectorIndex_) = resLog;

	if (typeHistory_.size() <= vectorIndex_)
		typeHistory_.resize(typeHistory_.size() + kNumPerResize_);
	typeHistory_.at(vectorIndex_) = type;

    if (severityHistory_.size() <= vectorIndex_)
		severityHistory_.resize(severityHistory_.size() + kNumPerResize_);
	severityHistory_.at(vectorIndex_) = severity;

	vectorIndex_ += 1;

	// Color in console
	if(doLogInConsole_)	{
		switch (severity) {
		case LogSeverity::ERROR:
			std::cout << "\x1B[31m" << resLog << "\033[0m\t\t" << std::endl; // 31m : red
			break;
		case LogSeverity::WARNING:
			std::cout << "\x1B[33m" << resLog << "\033[0m\t\t" << std::endl; // 33m : orange
			break;
		case LogSeverity::DEBUG:
		default :
			std::cout << resLog << std::endl; // Nothing : white
			break;
		}
	}
}

void Logger::ClearConsole() const {
	system("cls");
}

void Logger::EnableFileLogger(bool newValue) {
	doLogInFile_ = newValue;
}

void Logger::EnableConsoleLogger(bool newValue) {
	doLogInConsole_ = newValue;
}

void Logger::WriteLogsInFile() {

	if (!doLogInFile_) return;

	int year, month, day, hour, mins, secs;
	Time::Get().GetActualDate(year, month, day, hour, mins, secs);

	std::string currDateTime = 
		std::to_string(year) + "-" + 
		std::to_string(month) + "-" + 
		std::to_string(day) + "_" + 
		std::to_string(hour) + "-" + 
		std::to_string(mins) + "-" + 
		std::to_string(secs);
	
	std::string output;
	output += "--------------------------------------------------------------------------------\n";
	output += "| Project was run from the project : '";
	output += PROJECT_NAME;
	output += "' at " + currDateTime + " | \n";
	output += "--------------------------------------------------------------------------------\n\n";
	
	for(auto& s : logHistory_) {
		if (s.length()) {   
			output += s + "\n";
		} else {   
			break;
		}
	}
	PokFileSystem::WriteFile(currDateTime, output, FileType::LOGS);
}

const char * Logger::GetLogTypeString(LogType logType)
{
	if (logType == LogType::ALL_LOG) {   
		return "All";
	}

    switch(logType) {
	case LogType::DEFAULT_LOG: return "Default";
	case LogType::GRAPHICS_LOG: return "Graphics";
	case LogType::PHYSICS_LOG: return "Physics";
	case LogType::AUDIO_LOG: return "Audio";
	case LogType::INPUT_LOG: return "Input";
	case LogType::ECS_LOG: return "Ecs";
	case LogType::SCENE_LOG: return "Scene";
	case LogType::TOOL_LOG: return "Tool";
	case LogType::EDITOR_LOG: return "Editor";
	default: return "";
    }
}

void LogDebug(const std::string& msg, LogType logType) {
#ifndef NO_PROFILING
	Logger::Get().WriteLog(msg, logType, LogSeverity::DEBUG);
#endif
}

void LogWarning(const std::string& msg, LogType logType) {
#ifndef NO_PROFILING
	Logger::Get().WriteLog(msg, logType, LogSeverity::WARNING);
#endif
}

void LogError(const std::string& msg, LogType logType) {
#ifndef NO_PROFILING
	Logger::Get().WriteLog(msg, logType, LogSeverity::ERROR);
#endif
}

void ClearConsole() {
	Logger::Get().ClearConsole();
}

const std::vector<std::string>& GetLogs() {
	return Logger::Get().GetLogHistory();
}

const std::vector<LogType>& GetTypeLogs() {
	return Logger::Get().GetTypeHistory();
}

const std::vector<LogSeverity>& GetSeverityLogs() {
	return Logger::Get().GetSeverityHistory();
}

size_t GetNumLogs() {
	return Logger::Get().GetNumLogs();
}

} // namespace poke
