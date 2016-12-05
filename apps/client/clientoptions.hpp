#ifndef OPENMW_CLIENT_CLIENTOPTIONS_H
#define OPENMW_CLIENT_CLIENTOPTIONS_H

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
                DM_FullscreenWindow =   1,
                DM_BorderlessWindow =   2
            };

            // Uses defaults, TODO: remove
            ClientOptions();

            // Loads from a file
            ClientOptions(std::istream& stream);

            const std::string& getWindowTitle();

            int getDisplayMode();

            int getDisplayCount();
            int getDisplayPositionX(int display);
            int getDisplayPositionY(int display);
            int getDisplayWidth(int display);
            int getDisplayHeight(int display);

            int getVSync();
            int getDoubleBuffer();

            int getRedBits();
            int getGreenBits();
            int getBlueBits();
            int getAlphaBits();

            int getStencilBits();
            int getDepthBits();

        private:

            std::string mWindowTitle;

            int mDisplayMode;

            int mDisplayCount;
            std::vector<int> mDisplayPosXs;
            std::vector<int> mDisplayPosYs;
            std::vector<int> mDisplayWidths;
            std::vector<int> mDisplayHeights;

            int mVSync;
            int mDoubleBuffer;

            int mRedBits;
            int mGreenBits;
            int mBlueBits;
            int mAlphaBits;

            int mStencilBits;
            int mDepthBits;
    };
}

#endif
