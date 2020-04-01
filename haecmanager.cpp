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

    speex_echo_ctl(mEchoState, SPEEX_ECHO_SET_SAMPLING_RATE, &mSamplingRate);

    mPreprocess = speex_preprocess_state_init(mFrameSize, mSamplingRate);
    speex_preprocess_ctl(mPreprocess, SPEEX_PREPROCESS_SET_ECHO_STATE, mEchoState);

//    //////////////////////
//    int i=1;
//    speex_preprocess_ctl(mPreprocess, SPEEX_PREPROCESS_SET_DENOISE, &i);

//    int noiseSuppress = -25;

//    speex_preprocess_ctl(mPreprocess, SPEEX_PREPROCESS_SET_NOISE_SUPPRESS, &noiseSuppress); //设置噪声的dB



//    i=0;
//    speex_preprocess_ctl(mPreprocess, SPEEX_PREPROCESS_SET_AGC, &i);  //增益
//    i=8000;
//    speex_preprocess_ctl(mPreprocess, SPEEX_PREPROCESS_SET_AGC_LEVEL, &i);


//    //静音检测

//    int vad = 1;
//    int vadProbStart = 80;
//    int vadProbContinue = 65;
//    speex_preprocess_ctl(mPreprocess, SPEEX_PREPROCESS_SET_VAD, &vad); //静音检测
//    speex_preprocess_ctl(mPreprocess, SPEEX_PREPROCESS_SET_PROB_START , &vadProbStart); //Set probability required for the VAD to go from silence to voice
//    speex_preprocess_ctl(mPreprocess, SPEEX_PREPROCESS_SET_PROB_CONTINUE, &vadProbContinue); //Set probability required for the VAD to stay in the voice state (integer percent)

//    i=0;
//    speex_preprocess_ctl(mPreprocess, SPEEX_PREPROCESS_SET_DEREVERB, &i);
//    float f=.0;
//    speex_preprocess_ctl(mPreprocess, SPEEX_PREPROCESS_SET_DEREVERB_DECAY, &f);
//    f=.0;
//    speex_preprocess_ctl(mPreprocess, SPEEX_PREPROCESS_SET_DEREVERB_LEVEL, &f);
//    //////////////////////
    mInitialized = true;

    qDebug() << "1 AEC Props -----------------------";
    qDebug() << "Frame Len:" << mFrameSize;
    qDebug() << "Filter Len:" << mFilterLen;
    qDebug() << "Sampling Rate:" << mSamplingRate;
    qDebug() << "---------------------------------";
    resetAec();
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
    //    speex_preprocess_estimate_update(mPreprocess, mMicBuffer);
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

void HAECManager::setSamplingRate(int samplingRate)
{
    mSamplingRate = samplingRate;
}
