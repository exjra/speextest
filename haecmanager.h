#ifndef HAECMANAGER_H
#define HAECMANAGER_H

#include <speex/speex.h>
#include <speex/speex_preprocess.h>
#include <speex/speex_echo.h>

class HAECManager
{
public:
    HAECManager();

    void init(int pFrameSize);
    void init(int pFrameSize, int pFilterLength);
    void deInit();

    void onPlayback(char* data);
    void onCapture(const char *data);

    bool initialized();

    void setFilterLen(int filterLen);
    void setSamplingRate(int samplingRate);

    char* getCleanBuffer();

    int getFrameSize();

    void resetAec();

    int getSamplingRate() const;

private:
    SpeexEchoState *mEchoState;
    SpeexPreprocessState *mPreprocess;

    spx_int16_t* mEarBuffer;
    spx_int16_t* mMicBuffer;
    spx_int16_t* mOutBuffer;
    int mFrameSize;
    int mFilterLen;
    int mSamplingRate;
    bool mInitialized;
};

#endif // HAECMANAGER_H
