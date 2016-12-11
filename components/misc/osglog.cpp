#include "osglog.hpp"
#include "log.hpp"

namespace Misc
{
    osg::ref_ptr<OSGLog> OSGLog::mInstance = nullptr;

    bool OSGLog::mEnabled = true;

    void OSGLog::setup()
    {
        if (!mInstance)
            mInstance = new OSGLog();

        osg::setNotifyHandler(mInstance);
    }

    void OSGLog::setEnabled(bool state)
    {
        mEnabled = state;
    }

    void OSGLog::notify(osg::NotifySeverity severity, const char* message)
    {
        if (!mEnabled)
            return;

        switch (severity)
        {
            case osg::DEBUG_INFO:
            case osg::DEBUG_FP:
                Log::get().writeDebugInfo("OSG", message);
                break;

            case osg::INFO:
                Log::get().writeStatusInfo("OSG", message);
                break;

            case osg::NOTICE:
            case osg::WARN:
                Log::get().writeWarning("OSG", message);
                break;

            case osg::FATAL:
            case osg::ALWAYS:
                Log::get().writeError("OSG", message);
                break;

            default:
                break;
        }
    }

    OSGLog::OSGLog()
    {
    }

    OSGLog::~OSGLog()
    {
    }
}
