#ifndef OPENMW_SERVER_SERVER_H
#define OPENMW_SERVER_SERVER_H

namespace MWServer
{
    class ServerOptions;

    class Server
    {
        public:

            Server();

            void init(const ServerOptions& options);
            void update();
            void cleanup();

        private:


    };
}

#endif
