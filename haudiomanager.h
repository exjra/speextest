#ifndef HAUDIOMANAGER_H
#define HAUDIOMANAGER_H

#include <QObject>
#include <QAudioInput>
#include <QFile>
#include <QAudioOutput>

#include "hdevice.h"
#include "hmicbuffer.h"
#include "hearbuffer.h"

#include "hmicbuffernetwork.h"
#include "hearbuffernetwork.h"

#include "haecmanager.h"


#if defined(HAS_HARFSDK)
#if !defined (__ANDROID__)
#include "haudioclient.h"
#endif
#endif

class HAudioManager : public QObject
{
    Q_OBJECT

public:
    HAudioManager();

    void initAudioSystem();

    void init();
    void deInit();
    void play();
    void stopPlay();
    void playRecord(QString pFile);
    void initWithAEC(int pFrameLenMs, int pFilterLenMs, int pInternalDelayMs);

#if defined(HAS_HARFSDK)
#if !defined (__ANDROID__)
    void initForNetwork(HAudioClient* pClient, int pFrameLenMs, int pFilterLenMs, int pInternalDelayMs, bool pAECResetEnabled);
#endif
#endif

    void deInitWithAEC();

    void setSampleRate(int bitRate);

    void resetAudioDevices();

    void initForInternalDelay();

    void setVolumes(float pMic, float pSpeaker);    

private:
    QAudioInput* mMicDevice;
    QAudioOutput* mEarDevice;
    HDevice destinationFile;
    QFile sourcefile;
    HMicBuffer* mMicBuffer;
    HEarBuffer* mEarBuffer;

    HMicBufferNetwork* mMicBufferNetwork;
    HEarBufferNetwork* mEarBufferNetwork;

    HAECManager* mEchoManager;
    int mSampleRate;
    float mMicVolume;
    float mSpeakerVolume;

private slots:
    void notifyEar();
    void handleStateChangedMic(QAudio::State newState);
    void handleStateChangedEar(QAudio::State newState);

signals:
    void onSpeechState(bool pSpeech);
    void onTimeDiff(int pDiff);

};

#endif // HAUDIOMANAGER_H
