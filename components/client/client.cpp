#include "client.hpp"
#include "clientoptions.hpp"

#include <components/misc/log.hpp>
#include <components/sdlutil/sdlgraphicswindow.hpp>

#include <osgViewer/Viewer>
#include <osg/GraphicsContext>

#include <SDL.h>

#include <stdexcept>

namespace MWClient
{
    Client::Client()
        : mWindow(nullptr)
    {
    }

    Client::~Client()
    {
    }

    void Client::init(const ClientOptions& options)
    {
        // SDL
        {
            Misc::Log::get().writeStatusInfo("Client", "Initialising SDL");

            Uint32 subsystems = 0;

            subsystems |= SDL_INIT_VIDEO;
            subsystems |= SDL_INIT_JOYSTICK;
            subsystems |= SDL_INIT_GAMECONTROLLER;

            SDL_SetMainReady();
            checkSDLError(SDL_Init(subsystems), "Failed to initialize SDL");
        }

        // Window
        {
            Misc::Log::get().writeStatusInfo("Client", "Creating SDL windows");

            // Retrieve settings
            std::string title = options.getWindowTitle();
            bool fullscreen = options.getDisplayMode() == ClientOptions::DM_FullScreenWindow;
            bool borderless = options.getDisplayMode() == ClientOptions::DM_BorderlessWindow;

            int screen = 0;
            int posX = options.getDisplayPositionX();
            int posY = options.getDisplayPositionY();
            int width = options.getDisplayWidth();
            int height = options.getDisplayHeight();

            bool vsync = options.getVSync();
            bool doubleBuffer = options.getDoubleBuffer();
            bool minimizeOnFocusLoss = options.getMinimizeOnFocusLoss();

            int antialiasing = options.getAntialiasing();

            int red = options.getRedBits();
            int blue = options.getBlueBits();
            int green = options.getGreenBits();
            int alpha = options.getAlphaBits();

            int depth = options.getDepthBits();
            int stencil = options.getStencilBits();

            Uint32 flags = 0;

            // Set up window/context options
            SDL_SetHint(SDL_HINT_VIDEO_MINIMIZE_ON_FOCUS_LOSS, minimizeOnFocusLoss ? "1" : "0");

            checkSDLError(SDL_GL_SetAttribute(SDL_GL_RED_SIZE, red));
            checkSDLError(SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, green));
            checkSDLError(SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, blue));
            checkSDLError(SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, alpha));
            checkSDLError(SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, depth));
            checkSDLError(SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, stencil));


            if (antialiasing > 0)
            {
                checkSDLError(SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1));
                checkSDLError(SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, antialiasing));
            }


            flags |= SDL_WINDOW_OPENGL;
            flags |= SDL_WINDOW_SHOWN;
            flags |= SDL_WINDOW_RESIZABLE;

            if (fullscreen)
                flags |= SDL_WINDOW_FULLSCREEN;

            if (borderless)
                flags |= SDL_WINDOW_BORDERLESS;

            // Create window
            mWindow = SDL_CreateWindow(title.c_str(), posX, posY, width, height, flags);

            if (!mWindow && antialiasing != 0)
            {
                Misc::Log::get().writeWarning("Window", "Window creation failed, trying without antialiasing");

                antialiasing = 0;
                checkSDLError(SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, antialiasing));

                mWindow = SDL_CreateWindow(title.c_str(), posX, posY, width, height, flags);
            }

            if (!mWindow)
            {
                Misc::Log::get().writeError("Window", "Window creation failed");
                throw std::runtime_error("Failed to create a window");
            }



            Misc::Log::get().writeStatusInfo("Client", "Initialising OSG");

            // Retrieve window options
            title = SDL_GetWindowTitle(mWindow);
            flags = SDL_GetWindowFlags(mWindow);
            borderless = flags & SDL_WINDOW_BORDERLESS;

            screen = SDL_GetWindowDisplayIndex(mWindow);
            SDL_GetWindowPosition(mWindow, &posX, &posY);
            SDL_GetWindowSize(mWindow, &width, &height);

            checkSDLError(SDL_GL_GetAttribute(SDL_GL_RED_SIZE, &red));
            checkSDLError(SDL_GL_GetAttribute(SDL_GL_GREEN_SIZE, &green));
            checkSDLError(SDL_GL_GetAttribute(SDL_GL_BLUE_SIZE, &blue));
            checkSDLError(SDL_GL_GetAttribute(SDL_GL_ALPHA_SIZE, &alpha));
            checkSDLError(SDL_GL_GetAttribute(SDL_GL_DEPTH_SIZE, &depth));
            checkSDLError(SDL_GL_GetAttribute(SDL_GL_STENCIL_SIZE, &stencil));

            // Convert info into a form OSG understands
            osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits();

            traits->windowName = title;
            traits->windowDecoration = !borderless;

            traits->screenNum = screen;
            traits->x = posX;
            traits->y = posY;
            traits->width = width;
            traits->height = height;

            traits->red = red;
            traits->green = green;
            traits->blue = blue;
            traits->alpha = alpha;
            traits->depth = depth;
            traits->stencil = stencil;

            traits->vsync = vsync;
            traits->doubleBuffer = doubleBuffer;

            traits->inheritedWindowData = new SDLUtil::SDLGraphicsWindow::WindowData(mWindow);

            // Set up a gl context for OSG
            osg::ref_ptr<SDLUtil::SDLGraphicsWindow> graphicsWindow = new SDLUtil::SDLGraphicsWindow(traits);
            if (!graphicsWindow->valid())
            {
                Misc::Log::get().writeError("OSG", "Failed to create graphics context");
                std::runtime_error("Failed to intitialize");
            }

            // Setup rendering
            mViewer = new osgViewer::Viewer();

            osg::ref_ptr<osg::Camera> camera = mViewer->getCamera();
            camera->setGraphicsContext(graphicsWindow);
            camera->setViewport(0, 0, width, height);

            mViewer->realize();

            // temp
            mInputWrapper = std::shared_ptr<SDLUtil::InputWrapper>(new SDLUtil::InputWrapper(mWindow, false));

            mWindowState = std::shared_ptr<WindowState>(new WindowState());
            mInputWrapper->setWindowEventCallback(mWindowState.get());
        }
    }

    void Client::update()
    {
        mViewer->frame();

        mWindowState->reset();
        mInputWrapper->capture(false);

        if (mWindowState->wasResized())
        {
            int x, y, w, h;
            SDL_GetWindowPosition(mWindow, &x, &y);
            SDL_GetWindowSize(mWindow, &w, &h);
            mViewer->getCamera()->getGraphicsContext()->resized(x, y, w, h);
            mViewer->getEventQueue()->windowResize(x, y, w, h);
        }
    }

    void Client::cleanup()
    {
        mInputWrapper.reset();
        mWindowState.reset();

        mViewer->setDone(true);
        mViewer = 0;

        SDL_DestroyWindow(mWindow);
        SDL_Quit();
    }

    void Client::connect(std::shared_ptr<MWServer::Server> server)
    {
    }

    bool Client::shouldShutdown()
    {
        return mWindowState->wasQuit();
    }

    void Client::checkSDLError(int code)
    {
        if (code < 0)
            Misc::Log::get().writeError("SDL", SDL_GetError());
    }

    void Client::checkSDLError(int code, const char* throwMessage)
    {
        if (code < 0)
        {
            Misc::Log::get().writeError("SDL", SDL_GetError());
            throw std::runtime_error(throwMessage);
        }
    }
}
