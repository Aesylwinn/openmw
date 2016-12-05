#include "client.hpp"
#include "clientoptions.hpp"

#include <stdexcept>
#include <SDL.h>

namespace MWClient
{
    Client::Client()
    {
    }

    Client::~Client()
    {
    }

    void Client::init(const ClientOptions& options)
    {
        // SDL
        {
            Uint32 subsystems = 0;
            subsystems |= SDL_INIT_VIDEO;
            subsystems |= SDL_INIT_JOYSTICK;
            subsystems |= SDL_INIT_GAMECONTROLLER;

            if (SDL_Init(subsystems) < 0)
            {
                // TODO: log error
                throw std::runtime_error("Failed to initialize SDL");
            }
        }
    }

    void Client::update()
    {
    }

    void Client::cleanup()
    {
        SDL_Quit();
    }

    void Client::connect(std::shared_ptr<MWServer::Server> server)
    {
    }
}
