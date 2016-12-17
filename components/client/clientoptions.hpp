#ifndef OPENMW_COMPONENTS_CLIENT_CLIENTOPTIONS_H
#define OPENMW_COMPONENTS_CLIENT_CLIENTOPTIONS_H

#include <string>
#include <vector>

namespace MWClient
{
    class ClientOptions
    {
        public:

            enum DisplayMode
            {
                DM_None             =  -1,
                DM_StandardWindow   =   0,
                DM_FullScreenWindow =   1,
                DM_BorderlessWindow =   2
            };

            // Uses defaults, TODO: remove
            ClientOptions();

            // Loads from a file
            ClientOptions(std::istream& stream);

            const std::string& getWindowTitle() const;

            int getDisplayMode() const;
            int getDisplayPositionX() const;
            int getDisplayPositionY() const;
            int getDisplayWidth() const;
            int getDisplayHeight() const;

            int getVSync() const;
            int getDoubleBuffer() const;
            int getAntialiasing() const;

            int getRedBits() const;
            int getGreenBits() const;
            int getBlueBits() const;
            int getAlphaBits() const;

            int getStencilBits() const;
            int getDepthBits() const;

            int getMinimizeOnFocusLoss() const;

        private:

            std::string mWindowTitle;

            int mDisplayMode;
            int mDisplayPosX;
            int mDisplayPosY;
            int mDisplayWidth;
            int mDisplayHeight;

            int mVSync;
            int mDoubleBuffer;
            int mAntialiasing;

            int mRedBits;
            int mGreenBits;
            int mBlueBits;
            int mAlphaBits;

            int mStencilBits;
            int mDepthBits;

            int mMinimizeOnFocusLoss;
    };
}

#endif
