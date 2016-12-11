#ifndef OPENMW_COMPONENTS_SDLUTIL_SDLGRAPHICSWINDOW_H
#define OPENMW_COMPONENTS_SDLUTIL_SDLGRAPHICSWINDOW_H

#include <osgViewer/GraphicsWindow>

#include <SDL_video.h>

namespace SDLUtil
{
    /// Allows the use of an SDL2 window with OSG, wraps window/context interaction
    class SDLGraphicsWindow : public osgViewer::GraphicsWindow
    {
        public:

            /// WindowData is used to pass in the SDL2 window handle attached to the GraphicsContext::Traits structure.
            struct WindowData : public osg::Referenced
            {
                WindowData(SDL_Window* window = nullptr);

                SDL_Window* mWindow;
            };

            SDLGraphicsWindow(osg::GraphicsContext::Traits* traits);

            virtual bool isSameKindAs(const Object* object) const;

            virtual const char* libraryName() const;
            virtual const char* className() const;

            virtual bool valid() const;
            /// Return true if the graphics context is ready to use.
            virtual bool isRealizedImplementation() const;

            virtual void setWindowName(const std::string& name);
            /// Makes windowed or borderless
            virtual bool setWindowDecorationImplementation(bool flag);
            virtual bool setWindowRectangleImplementation(int x, int y, int width, int height);
            virtual void setCursor(MouseCursor cursor);
            virtual void setSyncToVBlank(bool on);

            virtual void raiseWindow();

            virtual void grabFocus();
            virtual void grabFocusIfPointerInWindow();

            virtual bool realizeImplementation();
            virtual void closeImplementation();

            virtual bool makeCurrentImplementation();
            virtual bool releaseContextImplementation();

            virtual void swapBuffersImplementation();

        private:

            virtual ~SDLGraphicsWindow();

            void init();

            SDL_Window* mWindow;
            SDL_GLContext mContext;

            bool mValid;
            bool mRealized;
            bool mOwnsWindow;
    };

}

#endif /* OSGGRAPHICSWINDOW_H */
