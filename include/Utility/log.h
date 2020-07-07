//----------------------------------------------------------------------------------
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
// Date : 10.12.2019
//----------------------------------------------------------------------------------

#pragma once
#include <vector>
#include <mutex>

#if !defined PROJECT_NAME
	#define PROJECT_NAME ""
#endif

namespace poke {

/**
 * \brief Types of logs available.
 */
enum class LogSeverity : char {
	DEBUG = 0,
	WARNING,
	ERROR
};

enum class LogType : uint16_t {
	DEFAULT_LOG = 1 << 0,
	GRAPHICS_LOG = 1 << 1,
	PHYSICS_LOG = 1 << 2,
    INPUT_LOG   = 1 << 3,
	ECS_LOG     = 1 << 4,
	AUDIO_LOG   = 1 << 5,
	SCENE_LOG   = 1 << 6,
    TOOL_LOG    = 1 << 7,
	EDITOR_LOG  = 1 << 8,
	ALL_LOG     = (1 << 9) - 1
};

/**
 * \brief Static class Log : Used to put messages in a console, or in a file.
 */
class Logger {
public:
	Logger(Logger& log) = delete;
	~Logger() = default;

	/**
	 * \brief Get the Logger singleton
	 */
	static Logger& Get();

	/**
	 * \brief Write a log on the console.
	 * \param s The message to display on the console
	 * \param type Type of the message. Default is ALL_LOG
	 * \param severity Severity of the message. Default is DEBUG
	 */
	void WriteLog(
		const std::string& s, 
		LogType type = LogType::ALL_LOG, 
		LogSeverity severity = LogSeverity::DEBUG);
	
	/**
	 * \brief Clear the console (PC only)
	 */
	void ClearConsole() const;

	/**
	 * \brief Enable or disable the logging in file
	 */
	void EnableFileLogger(bool newValue);

	/**
	 * \brief Enable or disable the logging in console
	 */
	void EnableConsoleLogger(bool newValue);

	/**
	 * \brief  Write all the stored logs inside of a file (named year-month-day_hour-min-sec).
	 * Usually called at the end of the application.
	 */
	void WriteLogsInFile();
	
	/**
	 * \brief Getter of the history of all the logs done in this runtime
	 * \return all the logs done in runtime
	 */
	std::vector<std::string>& GetLogHistory() {
		return logHistory_;
	}	
	
	/**
	 * \brief Getter of the history of all the type of logs done in this runtime
	 * \return all the types of logs done in runtime
	 */
	std::vector<LogType>& GetTypeHistory() {
		return typeHistory_;
	}

	std::vector<LogSeverity>& GetSeverityHistory() {
		return severityHistory_;
	}

	/**
	 * \brief Getter of the number all the logs saved in this runtime
	 * \return number of logs made
	 */
	size_t GetNumLogs() const {
		return vectorIndex_;
	}

    /**
     * \brief Getter of the type in const char*.
     * \param logType Log type you want in const char*
     * \return
     */
	const char* GetLogTypeString(LogType logType);

private:
	Logger() = default;

	std::vector<LogSeverity> severityHistory_;
	std::vector<LogType> typeHistory_;
	std::vector<std::string> logHistory_;

	std::mutex logMutex_;

	const short kNumPerResize_ = 1000;
	size_t vectorIndex_;

	bool doLogInConsole_;
	bool doLogInFile_;
};

/**
 * \brief Log a message in the console
 * \param logType Type of log. Default is "ALL_LOG"
 * \param msg
 */
void LogDebug(const std::string& msg, LogType logType = LogType::DEFAULT_LOG);

/**
 * \brief Log a warning message in the console
 * \param logType Type of log. Default is "ALL_LOG"
 * \param msg
 */
void LogWarning(const std::string& msg, LogType logType  = LogType::DEFAULT_LOG);

/**
 * \brief Log an error message in the console
 * \param logType Type of log. Default is "ALL_LOG"
 * \param msg
 */
void LogError(const std::string& msg, LogType logType = LogType::DEFAULT_LOG);

/**
 * \brief Clear the console (Windows only)
 */
void ClearConsole();

/**
 * \brief Getter of the history of the logs
 * \return 
 */
const std::vector<std::string>& GetLogs();

/**
 * \brief Getter of the history type of the logs
 * \return 
 */
const std::vector<LogType>& GetTypeLogs();

/**
 * \brief Getter of the history type of the logs
 * \return
 */
const std::vector<LogSeverity>& GetSeverityLogs();

/**
 * \brief Getter of the number of logs made so far
 * \return 
 */
size_t GetNumLogs();

} //namespace poke
