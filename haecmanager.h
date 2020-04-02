#ifndef HAECMANAGER_H
#define HAECMANAGER_H

#include <speex/speex.h>
#include <speex/speex_preprocess.h>
#include <speex/speex_echo.h>

class HAECManager
{
public:
    HAECManager();

    void init();
    void deInit();

    void onPlayback(char* data);
    void onCapture(const char *data);

    bool initialized();

    void setSamplingRate(int samplingRate);
    int getSamplingRate() const;

    char* getCleanBuffer();
    int getFrameSize();

    void setEarReady(bool earReady);
    bool getEarReady() const;

    int getFilterLenMs() const;
    void setFilterLenMs(int filterLenMs);

    int getFilterLen() const;

    void resetAec();

    int getFrameSizeMs() const;
    void setFrameSizeMs(int frameSizeMs);

    int getInternalDelayLenMs() const;
    void setInternalDelayLenMs(int internalDelayLenMs);

    int getInternalDelayLen() const;
    void setInternalDelayLen(int internalDelayLen);

    int calculateAudioBufferLength();

private:
    void initPrivate(int pFrameSize, int pFilterLength, int pInternalDelayLenght);

    SpeexEchoState *mEchoState;
    SpeexPreprocessState *mPreprocess;

    spx_int16_t* mEarBuffer;
    spx_int16_t* mMicBuffer;
    spx_int16_t* mOutBuffer;

    int mFrameSize;
    int mFrameSizeMs;

    int mFilterLen;
    int mFilterLenMs;

    int mInternalDelayLen;
    int mInternalDelayLenMs;

    int mSamplingRate;

    bool mInitialized;
    bool mEarReady;
};

#endif // HAECMANAGER_H
