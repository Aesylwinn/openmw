#include "client.hpp"
#include "clientoptions.hpp"

#include <components/misc/log.hpp>

#include <fstream>

// TODO: eventually change this into a shared library
int main(int argc, char* argv[])
{
    std::ofstream logFile("client.log");
    Misc::Log::create(Misc::Log::LV_DebugInfo, logFile);

    MWClient::ClientOptions clientOptions;
    MWClient::Client client;

    client.init(clientOptions);

    client.cleanup();

    Misc::Log::destroy();

    return 0;
}
