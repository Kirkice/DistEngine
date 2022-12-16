/***********************************************************************************************
 ***							C O R E  ---  D I S T E N G I N E                            ***
 ***********************************************************************************************
 *                                                                                             *
 *                                   Project Name : Core									   *
 *                                                                                             *
 *                                   File Name : Log.cpp                                       *
 *                                                                                             *
 *                                    Programmer : Kirk                                        *
 *                                                                                             *
 *                                     Date : 2022/8/2                                         *
 *                                                                                             *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#include "Log.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace LogSystem
{
    std::shared_ptr<spdlog::logger> Log::CoreLogger;
    std::shared_ptr<spdlog::logger> Log::ClientLogger;

    void Log::Init()
    {
        spdlog::set_pattern("%^[%T] %n: %v%$");

        CoreLogger = spdlog::stdout_color_mt("Dist Engine");
        CoreLogger->set_level(spdlog::level::trace);

        ClientLogger = spdlog::stdout_color_mt("Application");
        ClientLogger->set_level(spdlog::level::trace);
    }

    void Log::Debug(std::string string)
    {
        Log::GetCoreLogger()->info(string);
    }

    void Log::DebugWarn(std::string string)
    {
        Log::GetCoreLogger()->warn(string);
    }

    void Log::DebugError(std::string string)
    {
        Log::GetCoreLogger()->error(string);
    }
}