#include "sdlinputwrapper.hpp"

namespace SDLUtil
{

    InputWrapper::InputWrapper(SDL_Window* window, bool grab)
        : mSDLWindow(window)
        , mMouseListener(nullptr)
        , mKeyboardListener(nullptr)
        , mWindowListener(nullptr)
        , mControllerListener(nullptr)
        , mWarpX(0)
        , mWarpY(0)
        , mWarpCompensate(false)
        , mWrapPointer(false)
        , mAllowGrab(grab)
        , mWantMouseVisible(false)
        , mWantGrab(false)
        , mWantRelative(false)
        , mGrabPointer(false)
        , mMouseRelative(false)
        , mFirstMouseMove(true)
        , mMouseZ(0)
        , mMouseX(0)
        , mMouseY(0)
        , mWindowHasFocus(true)
        , mMouseInWindow(true)
    {
        Uint32 flags = SDL_GetWindowFlags(mSDLWindow);
        mWindowHasFocus = (flags & SDL_WINDOW_INPUT_FOCUS);
        mMouseInWindow = (flags & SDL_WINDOW_MOUSE_FOCUS);
    }

    InputWrapper::~InputWrapper()
    {
    }

    void InputWrapper::setMouseEventCallback(MouseListener* listen)
    {
        mMouseListener = listen;
    }

    void InputWrapper::setKeyboardEventCallback(KeyListener* listen)
    {
        mKeyboardListener = listen;
    }

    void InputWrapper::setWindowEventCallback(WindowListener* listen)
    {
        mWindowListener = listen;
    }

    void InputWrapper::setControllerEventCallback(ControllerListener* listen)
    {
        mControllerListener = listen;
    }

    void InputWrapper::setMouseVisible(bool visible)
    {
        mWantMouseVisible = visible;
        updateMouseSettings();
    }

    void InputWrapper::setMouseRelative(bool relative)
    {
        mWantRelative = relative;
        updateMouseSettings();
    }

    void InputWrapper::setGrabPointer(bool grab)
    {
        mWantGrab = grab;
        updateMouseSettings();
    }

    bool InputWrapper::getMouseRelative()
    {
        return mMouseRelative;
    }

    void InputWrapper::updateMouseSettings()
    {
        mGrabPointer = mWantGrab && mMouseInWindow && mWindowHasFocus;
        SDL_SetWindowGrab(mSDLWindow, mGrabPointer && mAllowGrab ? SDL_TRUE : SDL_FALSE);

        SDL_ShowCursor(mWantMouseVisible || !mWindowHasFocus);

        bool relative = mWantRelative && mMouseInWindow && mWindowHasFocus;
        if(mMouseRelative == relative)
            return;

        mMouseRelative = relative;

        mWrapPointer = false;

        // eep, wrap the pointer manually if the input driver doesn't support
        // relative positioning natively
        // also use wrapping if no-grab was specified in options (SDL_SetRelativeMouseMode
        // appears to eat the mouse cursor when pausing in a debugger)
        bool success = mAllowGrab && SDL_SetRelativeMouseMode(relative ? SDL_TRUE : SDL_FALSE) == 0;
        if(relative && !success)
            mWrapPointer = true;

        //now remove all mouse events using the old setting from the queue
        SDL_PumpEvents();
        SDL_FlushEvent(SDL_MOUSEMOTION);
    }

    void InputWrapper::capture(bool windowEventsOnly)
    {
        SDL_Event evt;

        if (windowEventsOnly)
        {
            // Needs to be manually called when peeping
            SDL_PumpEvents();

            // During loading, just handle window events, and keep others for later
            while (SDL_PeepEvents(&evt, 1, SDL_GETEVENT, SDL_WINDOWEVENT, SDL_WINDOWEVENT))
                handleWindowEvent(evt);
            return;
        }

        while(SDL_PollEvent(&evt))
        {
            switch(evt.type)
            {
                case SDL_MOUSEMOTION:
                    // Ignore this if it happened due to a warp
                    if (!handleWarpMotion(evt.motion))
                    {
                        // If in relative mode, don't trigger events unless window has focus
                        if (mMouseListener && (!mWantRelative || mWindowHasFocus))
                            mMouseListener->mouseMoved(evt.motion);

                        // Try to keep the mouse inside the window
                        if (mWindowHasFocus)
                            wrapMousePointer(evt.motion);
                    }
                    break;

                case SDL_MOUSEWHEEL:
                    if (mMouseListener)
                        mMouseListener->mouseScrolled(evt.wheel);
                    break;

                case SDL_MOUSEBUTTONDOWN:
                    if (mMouseListener)
                        mMouseListener->mousePressed(evt.button);
                    break;

                case SDL_MOUSEBUTTONUP:
                    if (mMouseListener)
                        mMouseListener->mouseReleased(evt.button);
                    break;

                case SDL_KEYDOWN:
                    if (mKeyboardListener && !evt.key.repeat)
                        mKeyboardListener->keyPressed(evt.key);
                    break;

                case SDL_KEYUP:
                    if (mKeyboardListener && !evt.key.repeat)
                        mKeyboardListener->keyReleased(evt.key);
                    break;

                case SDL_TEXTINPUT:
                    if (mKeyboardListener)
                        mKeyboardListener->textInput(evt.text);
                    break;

                case SDL_JOYHATMOTION: //As we manage everything with GameController, don't even bother with these.
                case SDL_JOYAXISMOTION:
                case SDL_JOYBUTTONDOWN:
                case SDL_JOYBUTTONUP:
                case SDL_JOYDEVICEADDED:
                case SDL_JOYDEVICEREMOVED:
                    break;

                case SDL_CONTROLLERDEVICEADDED:
                    if (mControllerListener)
                        mControllerListener->controllerAdded(evt.cdevice);
                    break;

                case SDL_CONTROLLERDEVICEREMOVED:
                    if (mControllerListener)
                        mControllerListener->controllerRemoved(evt.cdevice);
                    break;

                case SDL_CONTROLLERBUTTONDOWN:
                    if (mControllerListener)
                        mControllerListener->buttonPressed(evt.cbutton);
                    break;

                case SDL_CONTROLLERBUTTONUP:
                    if (mControllerListener)
                        mControllerListener->buttonReleased(evt.cbutton);
                    break;

                case SDL_CONTROLLERAXISMOTION:
                    if (mControllerListener)
                        mControllerListener->axisMoved(evt.caxis);
                    break;

                case SDL_WINDOWEVENT:
                    handleWindowEvent(evt);
                    break;

                case SDL_QUIT:
                    if (mWindowListener)
                        mWindowListener->sdlQuit(evt.quit);
                    break;

                case SDL_CLIPBOARDUPDATE:
                    break; // We don't need this event, clipboard is retrieved on demand

                case SDL_FINGERDOWN:
                case SDL_FINGERUP:
                case SDL_FINGERMOTION:
                case SDL_DOLLARGESTURE:
                case SDL_DOLLARRECORD:
                case SDL_MULTIGESTURE:
                    // No use for touch & gesture events
                    break;

                default:
                    break;
            }
        }
    }

    void InputWrapper::handleWindowEvent(const SDL_Event& evt)
    {
        switch (evt.window.event)
        {
            case SDL_WINDOWEVENT_ENTER:
                mMouseInWindow = true;
                updateMouseSettings();
                break;

            case SDL_WINDOWEVENT_LEAVE:
                mMouseInWindow = false;
                updateMouseSettings();
                break;

            case SDL_WINDOWEVENT_MOVED:
                // I'm not sure what OSG is using the window position for, but I don't think it's needed,
                // so we ignore window moved events (improves window movement performance)
                break;

            case SDL_WINDOWEVENT_SIZE_CHANGED:
                if (mWindowListener)
                    mWindowListener->windowResized(evt.window);

                break;

            case SDL_WINDOWEVENT_RESIZED:
                // This should also fire SIZE_CHANGED, so no need to handle
                break;

            case SDL_WINDOWEVENT_FOCUS_GAINED:
                mWindowHasFocus = true;
                updateMouseSettings();
                if (mWindowListener)
                    mWindowListener->windowFocusGained(evt.window);
                break;

            case SDL_WINDOWEVENT_FOCUS_LOST:
                mWindowHasFocus = false;
                updateMouseSettings();
                if (mWindowListener)
                    mWindowListener->windowFocusLost(evt.window);
                break;

            case SDL_WINDOWEVENT_CLOSE:
                if (mWindowListener)
                    mWindowListener->windowClosed(evt.window);
                break;

            case SDL_WINDOWEVENT_SHOWN:
                if (mWindowListener)
                    mWindowListener->windowVisibilityGained(evt.window);
                break;

            case SDL_WINDOWEVENT_HIDDEN:
                if (mWindowListener)
                    mWindowListener->windowVisibilityLost(evt.window);
                break;
        }
    }

    bool InputWrapper::isModifierHeld(SDL_Keymod mod)
    {
        return (SDL_GetModState() & mod) != 0;
    }

    bool InputWrapper::isKeyDown(SDL_Scancode key)
    {
        return (SDL_GetKeyboardState(nullptr)[key]) != 0;
    }

    /// \brief Moves the mouse to the specified point within the viewport
    void InputWrapper::warpMouse(int x, int y)
    {
        SDL_WarpMouseInWindow(mSDLWindow, x, y);
        mWarpCompensate = true;
        mWarpX = x;
        mWarpY = y;
    }

    /// \brief Internal method for ignoring relative motions as a side effect
    ///        of warpMouse()
    bool InputWrapper::handleWarpMotion(const SDL_MouseMotionEvent& evt)
    {
        if(!mWarpCompensate)
            return false;

        //this was a warp event, signal the caller to eat it.
        if(evt.x == mWarpX && evt.y == mWarpY)
        {
            mWarpCompensate = false;
            return true;
        }

        return false;
    }

    /// \brief Wrap the mouse to the viewport
    void InputWrapper::wrapMousePointer(const SDL_MouseMotionEvent& evt)
    {
        //don't wrap if we don't want relative movements, support relative
        //movements natively, or aren't grabbing anyways
        if(!mMouseRelative || !mWrapPointer || !mGrabPointer)
            return;

        int width = 0;
        int height = 0;

        SDL_GetWindowSize(mSDLWindow, &width, &height);

        const int FUDGE_FACTOR_X = width;
        const int FUDGE_FACTOR_Y = height;

        //warp the mouse if it's about to go outside the window
        if(evt.x - FUDGE_FACTOR_X < 0  || evt.x + FUDGE_FACTOR_X > width
            || evt.y - FUDGE_FACTOR_Y < 0 || evt.y + FUDGE_FACTOR_Y > height)
        {
            warpMouse(width / 2, height / 2);
        }
    }
}
