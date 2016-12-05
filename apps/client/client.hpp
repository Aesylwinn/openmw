#ifndef OPENMW_CLIENT_CLIENT_H
#define OPENMW_CLIENT_CLIENT_H

#include <memory>

namespace MWServer
{
    class Server;
}

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

        private:

            std::shared_ptr<MWServer::Server> mServer;
    };
}

#endif
