#ifndef OPENMW_COMPONENTS_MISC_OSGLOG_H
#define OPENMW_COMPONENTS_MISC_OSGLOG_H

#include <osg/Notify>
#include <osg/ref_ptr>

namespace Misc
{
    /// This class redirects osg messages to the Log class. Make sure it exists!
    class OSGLog : public osg::NotifyHandler
    {
        public:

            /// Creates and registers an instance as OSG's notify handler
            static void setup();
            /// Determines whether or not the object will log messages
            static void setEnabled(bool state);

            void notify(osg::NotifySeverity severity, const char* message);

        private:

            OSGLog();
            ~OSGLog();

            static osg::ref_ptr<OSGLog> mInstance;
            static bool mEnabled;
    };
}

#endif
