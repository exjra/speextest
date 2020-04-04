#include "haecmanager.h"

#include <memory>

#include <QDateTime>
#include <QDebug>

HAECManager::HAECManager() :
    mEarBuffer(nullptr)
  , mOutBuffer(nullptr)
  , mMicBuffer(nullptr)
  , mInitialized(false)
  , mEarReady(false)
  , mFilterLenMs(-1)
  , mFrameSizeMs(-1)
  , mInternalDelayLenMs(-1)
  , mSpeech(0)
  , mSpeechPrev(0)
  , mFirstEar(true)
  , mFirstMic(true)
  , mResetEnabled(false)
{

}

void HAECManager::init()
{
    if(mInitialized)
        return;

    //calculate frame len (in samples)
    int tTempFrameLen = 0;
    if(mFrameSizeMs == -1){
#if defined (__ANDROID__)
        mFrameSizeMs = 50;
#else
        mFrameSizeMs = 20;
#endif
    }
    tTempFrameLen = ((int) (((float)mSamplingRate) * ((float)mFrameSizeMs) / 1000.0f));

    //calculate filter len (in samples)
    int tTempFilterLen = 0;
    if(mFilterLenMs == -1){
#if defined (__ANDROID__)
        mFilterLenMs = 256;
#else
        mFilterLenMs = 128;
#endif
    }
    tTempFilterLen = ((int) (((float)mSamplingRate) * ((float)mFilterLenMs) / 1000.0f));

    //calculate filter len (in bytes)
    int tTempIntDelay = 0;
    if(mInternalDelayLenMs == -1){
#if defined (__ANDROID__)
        mInternalDelayLenMs = 400;
#else
        mInternalDelayLenMs = 20;
#endif
    }
    tTempIntDelay = ((int) (((float)mSamplingRate) * 2 * ((float)mInternalDelayLenMs) / 1000.0f)); //for S16LE format!

    initPrivate(tTempFrameLen, tTempFilterLen, tTempIntDelay);
}

void HAECManager::initPrivate(int pFrameSize, int pFilterLength, int pInternalDelayLenght)
{
    if(mInitialized)
        return;

    mFrameSize = pFrameSize;
    mFilterLen = pFilterLength;
    mInternalDelayLen = pInternalDelayLenght;

    mEarBuffer = new spx_int16_t[mFrameSize];
    mMicBuffer = new spx_int16_t[mFrameSize];
    mOutBuffer = new spx_int16_t[mFrameSize];

    mEchoState = speex_echo_state_init(mFrameSize, mFilterLen);//mFrameSize*10);

    speex_echo_ctl(mEchoState, SPEEX_ECHO_SET_SAMPLING_RATE, &mSamplingRate);

    mPreprocess = speex_preprocess_state_init(mFrameSize, mSamplingRate);
    speex_preprocess_ctl(mPreprocess, SPEEX_PREPROCESS_SET_ECHO_STATE, mEchoState);

    int i=1;
    speex_preprocess_ctl(mPreprocess, SPEEX_PREPROCESS_SET_DENOISE, &i);
    i=0;
    speex_preprocess_ctl(mPreprocess, SPEEX_PREPROCESS_SET_AGC, &i);
    i=8000;
    speex_preprocess_ctl(mPreprocess, SPEEX_PREPROCESS_SET_AGC_LEVEL, &i);
    i=0;
    speex_preprocess_ctl(mPreprocess, SPEEX_PREPROCESS_SET_DEREVERB, &i);
    float f=.0;
    speex_preprocess_ctl(mPreprocess, SPEEX_PREPROCESS_SET_DEREVERB_DECAY, &f);
    f=.0;
    speex_preprocess_ctl(mPreprocess, SPEEX_PREPROCESS_SET_DEREVERB_LEVEL, &f);

    int vad = 1;
    int vadProbStart = 80;
    int vadProbContinue = 65;
    speex_preprocess_ctl(mPreprocess, SPEEX_PREPROCESS_SET_VAD, &vad);
    speex_preprocess_ctl(mPreprocess, SPEEX_PREPROCESS_SET_PROB_START , &vadProbStart); //Set probability required for the VAD to go from silence to voice
    speex_preprocess_ctl(mPreprocess, SPEEX_PREPROCESS_SET_PROB_CONTINUE, &vadProbContinue); //Set probability required for the VAD to stay in the voice state (integer percent)




    //    int i=1;
    //    speex_preprocess_ctl(mPreprocess, SPEEX_PREPROCESS_SET_DENOISE, &i);

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

    qDebug() << "AEC Props -----------------------";
    qDebug() << "Sampling Rate:" << mSamplingRate << "/ 1000 ms";
    qDebug() << "Audio Device Buffers:" << calculateAudioBufferLength() << "/ 50 ms";
    qDebug() << "Frame Len:" << mFrameSize << "/" << mFrameSizeMs << "ms";
    qDebug() << "Filter Len:" << mFilterLen << "/" << mFilterLenMs << "ms";
    qDebug() << "Internal Delay:" << mInternalDelayLen << "/" << mInternalDelayLenMs << "ms";
    qDebug() << "---------------------------------";

    resetAec();
}

bool HAECManager::getResetEnabled() const
{
    return mResetEnabled;
}

void HAECManager::setResetEnabled(bool resetEnabled)
{
    mResetEnabled = resetEnabled;
}

int HAECManager::getInternalDelayLen() const
{
    return mInternalDelayLen;
}

void HAECManager::setInternalDelayLen(int internalDelayLen)
{
    mInternalDelayLen = internalDelayLen;
}

int HAECManager::calculateAudioBufferLength()
{
    return ((int) ((((float)mSamplingRate) * 2.0f) / 20.0f)); //50 ms buffer
}

int HAECManager::getInternalDelayLenMs() const
{
    return mInternalDelayLenMs;
}

void HAECManager::setInternalDelayLenMs(int internalDelayLenMs)
{
    mInternalDelayLenMs = internalDelayLenMs;
}

int HAECManager::getFrameSizeMs() const
{
    return mFrameSizeMs;
}

void HAECManager::setFrameSizeMs(int frameSizeMs)
{
    mFrameSizeMs = frameSizeMs;
}

void HAECManager::deInit()
{

}

void HAECManager::onPlayback(char *data)
{
    if(mEarBuffer == nullptr)
        return;

    memcpy((char*) mEarBuffer, data, mFrameSize*2);

    if(mFirstEar)
    {
        mFirstEar = false;
        mFirstEarTime = QDateTime::currentMSecsSinceEpoch();
        qDebug() << "Ear First Time:" << mFirstEarTime;

        if(!mFirstEar && !mFirstMic)
            emit onTimeDiff(mFirstEarTime - mFirstMicTime);
    }

    speex_echo_playback(mEchoState, mEarBuffer);
}

void HAECManager::onCapture(const char *data)
{
    if(mMicBuffer == nullptr || mOutBuffer == nullptr)
        return;

    memcpy((char*) mMicBuffer, data, mFrameSize*2);

    if(mFirstMic)
    {
        mFirstMic = false;
        mFirstMicTime = QDateTime::currentMSecsSinceEpoch();
        qDebug() << "Mic First Time:" << mFirstMicTime;

        if(!mFirstEar && !mFirstMic)
            emit onTimeDiff(mFirstEarTime - mFirstMicTime);
    }

    speex_echo_capture(mEchoState, mMicBuffer, mOutBuffer);
//    speex_preprocess_estimate_update(mPreprocess, mMicBuffer);
    int tRet = speex_preprocess_run(mPreprocess, mOutBuffer);

    if(mSpeech == 0 && tRet == 0) //susmaya devam
    {

    }
    else if(mSpeech == 0 && tRet == 1) //konusmaya basladi
        emit onSpeechState(true);
    else if(mSpeech == 1 && tRet == 1) //konusmaya devam
    {

    }
    else if(mSpeech == 1 && tRet == 0) //susmaya basladi
        emit onSpeechState(false);

    mSpeech = tRet;
}

bool HAECManager::initialized()
{
    return mInitialized;
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

int HAECManager::getSamplingRate() const
{
    return mSamplingRate;
}

void HAECManager::setEarReady(bool earReady)
{
    mEarReady = earReady;
}

bool HAECManager::getEarReady() const
{
    return mEarReady;
}

int HAECManager::getFilterLenMs() const
{
    return mFilterLenMs;
}

void HAECManager::setFilterLenMs(int filterLenMs)
{
    mFilterLenMs = filterLenMs;
}

int HAECManager::getFilterLen() const
{
    return mFilterLen;
}

void HAECManager::setSamplingRate(int samplingRate)
{
    mSamplingRate = samplingRate;
}
