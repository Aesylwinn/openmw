#include "clientoptions.hpp"

namespace MWClient
{
    ClientOptions::ClientOptions()
    {
        mWindowTitle = "MW Client";

        mDisplayMode = DM_StandardWindow;
        mDisplayPosX = 100;
        mDisplayPosY = 100;
        mDisplayWidth = 640;
        mDisplayHeight = 480;

        mVSync = 1;
        mDoubleBuffer = 1;
        mAntialiasing = 0;

        mRedBits = 8;
        mGreenBits = 8;
        mBlueBits = 8;
        mAlphaBits = 0;

        mStencilBits = 8;
        mDepthBits = 24;

        mMinimizeOnFocusLoss = 0;
    }

    ClientOptions::ClientOptions(std::istream& stream)
    {
        // TODO: load from file
    }

    const std::string& ClientOptions::getWindowTitle() const
    {
        return mWindowTitle;
    }

    int ClientOptions::getDisplayMode() const
    {
        return mDisplayMode;
    }

    int ClientOptions::getDisplayPositionX() const
    {
        return mDisplayPosX;
    }

    int ClientOptions::getDisplayPositionY() const
    {
        return mDisplayPosY;
    }

    int ClientOptions::getDisplayWidth() const
    {
        return mDisplayWidth;
    }

    int ClientOptions::getDisplayHeight() const
    {
        return mDisplayHeight;
    }

    int ClientOptions::getVSync() const
    {
        return mVSync;
    }

    int ClientOptions::getDoubleBuffer() const
    {
        return mDoubleBuffer;
    }

    int ClientOptions::getAntialiasing() const
    {
        return mAntialiasing;
    }

    int ClientOptions::getRedBits() const
    {
        return mRedBits;
    }

    int ClientOptions::getGreenBits() const
    {
        return mGreenBits;
    }

    int ClientOptions::getBlueBits() const
    {
        return mBlueBits;
    }

    int ClientOptions::getAlphaBits() const
    {
        return mAlphaBits;
    }

    int ClientOptions::getStencilBits() const
    {
        return mStencilBits;
    }

    int ClientOptions::getDepthBits() const
    {
        return mDepthBits;
    }

    int ClientOptions::getMinimizeOnFocusLoss() const
    {
        return mMinimizeOnFocusLoss;
    }
}
