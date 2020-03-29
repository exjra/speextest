#include "haecmanager.h"

#include <memory>

#include <QDebug>

HAECManager::HAECManager() :
    mEarBuffer(nullptr)
  , mOutBuffer(nullptr)
  , mMicBuffer(nullptr)
  , mInitialized(false)
{

}

void HAECManager::init(int pFrameSize)
{
    if(mInitialized)
        return;

    init(pFrameSize, mFilterLen);
}

void HAECManager::init(int pFrameSize, int pFilterLength)
{
    if(mInitialized)
        return;

    mFrameSize = pFrameSize;
    mFilterLen = pFilterLength;

    mEarBuffer = new spx_int16_t[mFrameSize];
    mMicBuffer = new spx_int16_t[mFrameSize];
    mOutBuffer = new spx_int16_t[mFrameSize];

    mEchoState = speex_echo_state_init(mFrameSize, mFilterLen);//mFrameSize*10);

    int mSamplingRate = 8000;
    speex_echo_ctl(mEchoState, SPEEX_ECHO_SET_SAMPLING_RATE, &mSamplingRate);

    mPreprocess = speex_preprocess_state_init(mFrameSize, mSamplingRate);
    speex_preprocess_ctl(mPreprocess, SPEEX_PREPROCESS_SET_ECHO_STATE, mEchoState);

    mInitialized = true;
}

void HAECManager::deInit()
{

}

void HAECManager::onPlayback(char *data)
{
    if(mEarBuffer == nullptr)
        return;

    memcpy((char*) mEarBuffer, data, mFrameSize*2);

    speex_echo_playback(mEchoState, mEarBuffer);
}

void HAECManager::onCapture(const char *data)
{
    if(mMicBuffer == nullptr || mOutBuffer == nullptr)
        return;

    memcpy((char*) mMicBuffer, data, mFrameSize*2);

    speex_echo_capture(mEchoState, mMicBuffer, mOutBuffer);
    speex_preprocess_run(mPreprocess, mOutBuffer);
}

bool HAECManager::initialized()
{
    return mInitialized;
}

void HAECManager::setFilterLen(int filterLen)
{
    mFilterLen = filterLen;
}

char *HAECManager::getCleanBuffer()
{
    if(mOutBuffer == nullptr)
        return nullptr;

    return (char*) mOutBuffer;
}

int HAECManager::getFrameSize()
{
    return mFrameSize;
}

void HAECManager::resetAec()
{
    speex_echo_state_reset(mEchoState);
}
