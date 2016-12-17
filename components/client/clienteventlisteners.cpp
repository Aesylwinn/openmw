#include "clienteventlisteners.hpp"

namespace MWClient
{
    WindowState::WindowState()
        : mClosed(false)
        , mFocused(false)
        , mQuit(false)
        , mResized(false)
        , mVisible(false)
    {
    }

    bool WindowState::wasClosed() const
    {
        return mClosed;
    }

    bool WindowState::wasQuit() const
    {
        return mQuit;
    }

    bool WindowState::wasResized() const
    {
        return mResized;
    }

    bool WindowState::isActive() const
    {
        return mFocused && mVisible;
    }

    void WindowState::reset()
    {
        mClosed = false;
        mQuit = false;
        mResized = false;
    }

    void WindowState::windowVisibilityGained(const SDL_WindowEvent& evt)
    {
        mVisible = true;
    }

    void WindowState::windowVisibilityLost(const SDL_WindowEvent& evt)
    {
        mVisible = false;
    }

    void WindowState::windowFocusGained(const SDL_WindowEvent& evt)
    {
        mFocused = true;
    }

    void WindowState::windowFocusLost(const SDL_WindowEvent& evt)
    {
        mFocused = false;
    }

    void WindowState::windowClosed(const SDL_WindowEvent& evt)
    {
        mClosed = true;
    }

    void WindowState::windowResized(const SDL_WindowEvent& evt)
    {
        mResized = true;
    }

    void WindowState::sdlQuit(const SDL_QuitEvent& evt)
    {
        mQuit = true;
    }
}
