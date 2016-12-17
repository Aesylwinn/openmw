#ifndef OPENMW_COMPONENTS_CLIENT_CLIENT_H
#define OPENMW_COMPONENTS_CLIENT_CLIENT_H

#include "clienteventlisteners.hpp"

#include <components/sdlutil/sdlinputwrapper.hpp>

#include <osg/ref_ptr>

#include <memory>
#include <vector>

namespace MWServer
{
    class Server;
}

namespace osgViewer
{
    class Viewer;
}

class SDL_Window;

namespace MWClient
{
    class ClientOptions;

    /// The user interface for the game. Logic is performed on the "server."
    class Client
    {
        public:

            Client();
            ~Client();

            /// Used to initialize the client with specific options that will not change at runtime.
            void init(const ClientOptions& options);

            /// For use in a game loop. For a single player game, the client and server can be run in the same loop.
            void update();

            /// Frees reserved memory, placing the client in an uninitialized state.
            void cleanup();

            /// Establishes and maintains a connection to the server
            void connect(std::shared_ptr<MWServer::Server> server);

            bool shouldShutdown();

        private:

            void checkSDLError(int code);
            void checkSDLError(int code, const char* throwMessage);

            SDL_Window* mWindow;

            osg::ref_ptr<osgViewer::Viewer> mViewer;

            std::shared_ptr<SDLUtil::InputWrapper> mInputWrapper;
            std::shared_ptr<WindowState> mWindowState;

            std::shared_ptr<MWServer::Server> mServer;
    };
}

#endif
