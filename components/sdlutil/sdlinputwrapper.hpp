#ifndef OPENMW_COMPONENTS_SDLUTIL_SDLINPUTWRAPPER_H
#define OPENMW_COMPONENTS_SDLUTIL_SDLINPUTWRAPPER_H

#include "sdleventlisteners.hpp"

#include <SDL_events.h>

#include <map>

namespace SDLUtil
{
    /// \brief A wrapper around SDL's event queue, mostly used for handling input-related events.
    class InputWrapper
    {
        public:
            InputWrapper(SDL_Window* window, bool grab);
            ~InputWrapper();

            void setMouseEventCallback(MouseListener* listen);
            void setKeyboardEventCallback(KeyListener* listen);
            void setWindowEventCallback(WindowListener* listen);
            void setControllerEventCallback(ControllerListener* listen);

            void setMouseVisible(bool visible);
            void setMouseRelative(bool relative);
            void setGrabPointer(bool grab);

            bool getMouseRelative();

            void updateMouseSettings();

            void capture(bool windowEventsOnly);
            bool isModifierHeld(SDL_Keymod mod);
            bool isKeyDown(SDL_Scancode key);

            void warpMouse(int x, int y);

        private:
            void handleWindowEvent(const SDL_Event& evt);

            bool handleWarpMotion(const SDL_MouseMotionEvent& evt);
            void wrapMousePointer(const SDL_MouseMotionEvent &evt);

            SDL_Window* mSDLWindow;

            MouseListener* mMouseListener;
            KeyListener* mKeyboardListener;
            WindowListener* mWindowListener;
            ControllerListener* mControllerListener;

            Uint16 mWarpX;
            Uint16 mWarpY;
            bool mWarpCompensate;
            bool mWrapPointer;

            bool mAllowGrab;
            bool mWantMouseVisible;
            bool mWantGrab;
            bool mWantRelative;
            bool mGrabPointer;
            bool mMouseRelative;

            bool mFirstMouseMove;

            Sint32 mMouseZ;
            Sint32 mMouseX;
            Sint32 mMouseY;

            bool mWindowHasFocus;
            bool mMouseInWindow;
    };

}

#endif
