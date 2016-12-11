#include "sdlgraphicswindow.hpp"

namespace SDLUtil
{
    SDLGraphicsWindow::WindowData::WindowData(SDL_Window* window)
        : mWindow(window)
    {
    }

    SDLGraphicsWindow::SDLGraphicsWindow(osg::GraphicsContext::Traits* traits)
        : mWindow(0)
        , mContext(0)
        , mValid(false)
        , mRealized(false)
        , mOwnsWindow(false)
    {
        _traits = traits;

        init();
        if (valid())
        {
            setState(new osg::State());
            getState()->setGraphicsContext(this);

            if(_traits.valid() && _traits->sharedContext.valid())
            {
                getState()->setContextID(_traits->sharedContext->getState()->getContextID());
                incrementContextIDUsageCount(getState()->getContextID());
            }
            else
            {
                getState()->setContextID(osg::GraphicsContext::createNewContextID());
            }
        }
    }

    SDLGraphicsWindow::~SDLGraphicsWindow()
    {
        close(true);
    }

    bool SDLGraphicsWindow::isSameKindAs(const Object* object) const
    {
        return (dynamic_cast<const SDLGraphicsWindow*>(object) != 0);
    }

    const char* SDLGraphicsWindow::libraryName() const
    {
        return "osgViewer";
    }

    const char* SDLGraphicsWindow::className() const
    {
        return "SDLGraphicsWindow";
    }

    bool SDLGraphicsWindow::valid() const
    {
        return mValid;
    }

    bool SDLGraphicsWindow::isRealizedImplementation() const
    {
        return mRealized;
    }

    void SDLGraphicsWindow::setWindowName(const std::string &name)
    {
        if (!mWindow)
            return;

        _traits->windowName = name;
        SDL_SetWindowTitle(mWindow, name.c_str());
    }

    bool SDLGraphicsWindow::setWindowDecorationImplementation(bool flag)
    {
        if (!mWindow)
            return false;

        SDL_SetWindowBordered(mWindow, flag ? SDL_TRUE : SDL_FALSE);

        return true;
    }

    bool SDLGraphicsWindow::setWindowRectangleImplementation(int x, int y, int width, int height)
    {
        if (!mWindow)
            return false;

        SDL_SetWindowPosition(mWindow, x, y);
        SDL_SetWindowSize(mWindow, width, height);

        return true;
    }

    void SDLGraphicsWindow::setCursor(MouseCursor mouseCursor)
    {
        _traits->useCursor = false;
    }

    void SDLGraphicsWindow::setSyncToVBlank(bool on)
    {
        SDL_Window* oldWin = SDL_GL_GetCurrentWindow();
        SDL_GLContext oldCtx = SDL_GL_GetCurrentContext();

        SDL_GL_MakeCurrent(mWindow, mContext);

        SDL_GL_SetSwapInterval(on ? 1 : 0);

        SDL_GL_MakeCurrent(oldWin, oldCtx);
    }

    void SDLGraphicsWindow::raiseWindow()
    {
        SDL_RaiseWindow(mWindow);
    }

    void SDLGraphicsWindow::grabFocus()
    {
        SDL_SetWindowGrab(mWindow, SDL_TRUE);
    }

    void SDLGraphicsWindow::grabFocusIfPointerInWindow()
    {
        SDL_SetWindowGrab(mWindow, SDL_FALSE);
    }

    bool SDLGraphicsWindow::realizeImplementation()
    {
        if (mRealized)
        {
            OSG_NOTICE << "SDLGraphicsWindow::realizeImplementation() Already realized" << std::endl;
            return true;
        }

        if (!mValid)
            init();

        if (!mValid)
            return false;

        SDL_ShowWindow(mWindow);

        getEventQueue()->syncWindowRectangleWithGraphicsContext();

        mRealized = true;

        return true;
    }

    void SDLGraphicsWindow::closeImplementation()
    {
        if (mContext)
            SDL_GL_DeleteContext(mContext);
        mContext = nullptr;

        if (mWindow && mOwnsWindow)
            SDL_DestroyWindow(mWindow);
        mWindow = nullptr;

        mValid = false;
        mRealized = false;
    }

    bool SDLGraphicsWindow::makeCurrentImplementation()
    {
        if (!mRealized)
        {
            OSG_WARN << "SDLGraphicsWindow not realized, cannot do makeCurrent." << std::endl;
            return false;
        }

        return (SDL_GL_MakeCurrent(mWindow, mContext) == 0);
    }

    bool SDLGraphicsWindow::releaseContextImplementation()
    {
        if (!mRealized)
        {
            OSG_WARN << "SDLGraphicsWindow not realized, cannot do releaseContext." << std::endl;
            return false;
        }

        return (SDL_GL_MakeCurrent(nullptr, nullptr) == 0);
    }

    void SDLGraphicsWindow::swapBuffersImplementation()
    {
        if (!mRealized)
            return;

        SDL_GL_SwapWindow(mWindow);
    }

    void SDLGraphicsWindow::init()
    {
        if (mValid || !_traits.valid())
            return;

        WindowData* inheritedWindowData = dynamic_cast<WindowData*>(_traits->inheritedWindowData.get());
        mWindow = inheritedWindowData ? inheritedWindowData->mWindow : nullptr;

        if (mWindow == nullptr)
        {
            mOwnsWindow = true;
            OSG_FATAL << "No SDL window provided." << std::endl;
            return;
        }
        else
            mOwnsWindow = false;

        // SDL will change the current context when it creates a new one, so we
        // have to get the current one to be able to restore it afterward.
        SDL_Window* oldWin = SDL_GL_GetCurrentWindow();
        SDL_GLContext oldCtx = SDL_GL_GetCurrentContext();

#if defined(OPENGL_ES) || defined(ANDROID)
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 1);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
#endif

        mContext = SDL_GL_CreateContext(mWindow);
        if (!mContext)
        {
            OSG_FATAL << "Unable to create OpenGL graphics context: " << SDL_GetError() << std::endl;
            return;
        }

        SDL_GL_SetSwapInterval(_traits->vsync ? 1 : 0);

        SDL_GL_MakeCurrent(oldWin, oldCtx);

        mValid = true;

        getEventQueue()->syncWindowRectangleWithGraphicsContext();
    }

}
