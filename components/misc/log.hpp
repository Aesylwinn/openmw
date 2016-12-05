#ifndef OPENMW_COMPONENTS_MISC_LOG_H
#define OPENMW_COMPONENTS_MISC_LOG_H

#include <mutex>
#include <ostream>

namespace Misc
{
    /// A globally accessible log writing utility. All methods except construction/destruction are thread-safe.
    class Log
    {
        public:

            enum Level
            {
                LV_DebugInfo    = 0,
                LV_StatusInfo   = 1,
                LV_Warning      = 2,
                LV_Error        = 4
            };

            /// Creates a new log if none exists, otherwise throws an exception. The stream must exist for the lifetime
            /// of the log.
            static void create(Level writeLevel, std::ostream& stream);
            /// Can be used to close the active log
            static void destroy();

            /// Retrieves the active log.
            static Log& get();

            /// Causes the log to ignore any levels lower than the given paremeter.
            void setWriteLevel(Level level);

            void writeDebugInfo(const std::string& category, const std::string& message);
            void writeStatusInfo(const std::string& category, const std::string& message);
            void writeWarning(const std::string& category, const std::string& message);
            void writeError(const std::string& category, const std::string& message);

        private:

            Log(Level writeLevel, std::ostream& stream);
            ~Log();

            std::string getLevelString(int level);

            void writeImp(int level, const std::string& category, const std::string& message);

            static Log* mInstance;

            int mWriteLevel;

            std::ostream& mStream;
            std::mutex mStreamMutex;
    };
}

#endif
