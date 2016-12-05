#include "log.hpp"

namespace Misc
{
    // Static methods

    Log* Log::mInstance = nullptr;

    void Log::create(Level writeLevel, std::ostream& stream)
    {
        if (mInstance)
            throw std::logic_error("A Log instance already exists");
        else
            mInstance = new Log(writeLevel, stream);
    }

    void Log::destroy()
    {
        if (mInstance)
        {
            delete mInstance;
            mInstance = nullptr;
        }
    }

    Log& Log::get()
    {
        return *mInstance;
    }

    // Object methods

    void Log::setWriteLevel(Level writeLevel)
    {
        std::lock_guard<std::mutex> lock(mStreamMutex);
        mWriteLevel = writeLevel;
    }

    void Log::writeDebugInfo(const std::string& category, const std::string& message)
    {
        writeImp(LV_DebugInfo, category, message);
    }

    void Log::writeStatusInfo(const std::string& category, const std::string& message)
    {
        writeImp(LV_StatusInfo, category, message);
    }

    void Log::writeWarning(const std::string& category, const std::string& message)
    {
        writeImp(LV_Warning, category, message);
    }

    void Log::writeError(const std::string& category, const std::string& message)
    {
        writeImp(LV_Error, category, message);
    }

    Log::Log(Level writeLevel, std::ostream& stream)
        : mWriteLevel(writeLevel)
        , mStream(stream)
    {
        writeStatusInfo("Log", "Starting new log");
    }

    Log::~Log()
    {
        writeStatusInfo("Log", "Closing gracefully");
    }

    std::string Log::getLevelString(int level)
    {
        switch (level)
        {
            case LV_DebugInfo:
                return "Debug Info";
            case LV_StatusInfo:
                return "Status Info";
            case LV_Warning:
                return "Warning";
            case LV_Error:
                return "Error";
            default:
                return "Invalid level";
        }
    }

    void Log::writeImp(int level, const std::string& category, const std::string& message)
    {
        if (level >= mWriteLevel)
        {
            std::lock_guard<std::mutex> lock(mStreamMutex);
            mStream << getLevelString(level) << ": " << category << ": " << message << std::endl << std::flush;
        }
    }
}
