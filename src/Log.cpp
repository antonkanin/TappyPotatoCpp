#include "Log.hpp"

#include <iostream>

namespace tp
{
#ifdef __ANDROID__
#include <SDL_main.h>
#include <android/log.h>
#endif

enum class LogType
{
    Info,
    Error
};

void log(LogType logType, const std::string& tag, const std::string& error)
{
#ifdef __ANDROID__
    __android_log_print(ANDROID_LOG_ERROR, tag.c_str(), "%s", error.c_str());
#else
    if (LogType::Info == logType)
    {
        std::cout << error << std::endl;
    }
    else if (LogType::Error == logType)
    {
        std::cerr << error << std::endl;
    }
#endif
}

void logError(const std::string& message)
{
    log(LogType::Error, "TAPPY_ERROR", message);
}

void logInfo(const std::string& message)
{
    log(LogType::Info, "TAPPY_INFO", message);
}
} // namespace tp