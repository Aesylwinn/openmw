#include "clientoptions.hpp"

namespace MWClient
{
    ClientOptions::ClientOptions()
    {
        mWindowTitle = "MW Client";

        mDisplayMode = DM_StandardWindow;

        mDisplayCount = 1;
        mDisplayPosXs.push_back(100);
        mDisplayPosYs.push_back(100);
        mDisplayWidths.push_back(640);
        mDisplayHeights.push_back(480);

        mVSync = 1;
        mDoubleBuffer = 1;

        mRedBits = 8;
        mGreenBits = 8;
        mBlueBits = 8;
        mAlphaBits = 0;

        mStencilBits = 8;
        mDepthBits = 24;
    }

    ClientOptions::ClientOptions(std::istream& stream)
    {
        // TODO: load from file
    }

    const std::string& ClientOptions::getWindowTitle()
    {
        return mWindowTitle;
    }

    int ClientOptions::getDisplayMode()
    {
        return mDisplayMode;
    }

    int ClientOptions::getDisplayCount()
    {
        return mDisplayCount;
    }

    int ClientOptions::getDisplayPositionX(int display)
    {
        return mDisplayPosXs[display];
    }

    int ClientOptions::getDisplayPositionY(int display)
    {
        return mDisplayPosYs[display];
    }

    int ClientOptions::getVSync()
    {
        return mVSync;
    }

    int ClientOptions::getDoubleBuffer()
    {
        return mDoubleBuffer;
    }

    int ClientOptions::getRedBits()
    {
        return mRedBits;
    }

    int ClientOptions::getGreenBits()
    {
        return mGreenBits;
    }

    int ClientOptions::getBlueBits()
    {
        return mBlueBits;
    }

    int ClientOptions::getAlphaBits()
    {
        return mAlphaBits;
    }

    int ClientOptions::getStencilBits()
    {
        return mStencilBits;
    }

    int ClientOptions::getDepthBits()
    {
        return mDepthBits;
    }
}
