#ifndef OPENMW_COMPONENTS_SDLUTIL_SDLEVENTLISTENERS_H
#define OPENMW_COMPONENTS_SDLUTIL_SDLEVENTLISTENERS_H

#include <SDL_types.h>
#include <SDL_events.h>


namespace SDLUtil
{
    class MouseListener
    {
        public:
            virtual ~MouseListener() {}
            virtual void mouseMoved(const SDL_MouseMotionEvent& arg) = 0;
            virtual void mouseScrolled(const SDL_MouseWheelEvent& arg) = 0;
            virtual void mousePressed(const SDL_MouseButtonEvent& arg) = 0;
            virtual void mouseReleased(const SDL_MouseButtonEvent& arg) = 0;
    };

    class KeyListener
    {
        public:
            virtual ~KeyListener() {}
            virtual void textInput(const SDL_TextInputEvent& arg) {}
            virtual void keyPressed(const SDL_KeyboardEvent& arg) = 0;
            virtual void keyReleased(const SDL_KeyboardEvent& arg) = 0;
    };

    /// Joystick/Gamepad listener
    class ControllerListener
    {
        public:
            virtual ~ControllerListener() {}
            virtual void buttonPressed(const SDL_ControllerButtonEvent& evt) = 0;
            virtual void buttonReleased(const SDL_ControllerButtonEvent& evt) = 0;
            virtual void axisMoved(const SDL_ControllerAxisEvent& arg) = 0;
            virtual void controllerAdded(const SDL_ControllerDeviceEvent& arg) = 0;
            virtual void controllerRemoved(const SDL_ControllerDeviceEvent& arg) = 0;
    };

    class WindowListener
    {
        public:
            virtual ~WindowListener() {}
            virtual void windowVisibilityGained(const SDL_WindowEvent& evt) = 0;
            virtual void windowVisibilityLost(const SDL_WindowEvent& evt) = 0;
            virtual void windowFocusGained(const SDL_WindowEvent& evt) = 0;
            virtual void windowFocusLost(const SDL_WindowEvent& evt) = 0;
            virtual void windowClosed(const SDL_WindowEvent& evt) = 0;
            virtual void windowResized(const SDL_WindowEvent& evt) = 0;

            virtual void sdlQuit(const SDL_QuitEvent& evt) = 0;
    };
}

#endif
