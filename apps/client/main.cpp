#include "client.hpp"
#include "clientoptions.hpp"

#include <components/misc/log.hpp>
#include <components/misc/osglog.hpp>

#include <fstream>

// TODO: eventually change this into a shared library
int main(int argc, char* argv[])
{
    std::ofstream logFile("client.log");
    Misc::Log::create(Misc::Log::LV_DebugInfo, logFile);
    Misc::OSGLog::setup();

    MWClient::ClientOptions clientOptions;
    MWClient::Client client;

    client.init(clientOptions);

    while (!client.shouldShutdown())
    {
        client.update();
    }

    client.cleanup();

    Misc::OSGLog::setEnabled(false);
    Misc::Log::destroy();

    return 0;
}
