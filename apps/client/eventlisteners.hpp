#ifndef OPENMW_CLIENT_EVENTLISTENERS_H
#define OPENMW_CLIENT_EVENTLISTENERS_H

#include <components/sdlutil/sdleventlisteners.hpp>

namespace MWClient
{
    class WindowState : public SDLUtil::WindowListener
    {
        public:

            WindowState();

            bool wasClosed() const;
            bool wasQuit() const;
            bool wasResized() const;
            bool isActive() const;

            /// Resets state for closed, quit, and resized events
            void reset();

            // Events

            void windowVisibilityGained(const SDL_WindowEvent& evt);
            void windowVisibilityLost(const SDL_WindowEvent& evt);
            void windowFocusGained(const SDL_WindowEvent& evt);
            void windowFocusLost(const SDL_WindowEvent& evt);
            void windowClosed(const SDL_WindowEvent& evt);
            void windowResized(const SDL_WindowEvent& evt);

            void sdlQuit(const SDL_QuitEvent& evt);

        private:

            bool mClosed;
            bool mFocused;
            bool mQuit;
            bool mResized;
            bool mVisible;
    };
}

#endif
