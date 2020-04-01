#ifndef HAUDIOMANAGER_H
#define HAUDIOMANAGER_H

#include <QObject>
#include <QAudioInput>
#include <QFile>
#include <QAudioOutput>

#include "hdevice.h"
#include "hmicbuffer.h"
#include "hearbuffer.h"

#include "haecmanager.h"

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
    void initWithAEC();
    void deInitWithAEC();

    void setBitRate(int bitRate);
    void setFrameLenMs(int frameLenMs);

    void resetAudioDevices();
private:
    QAudioInput* mMicDevice;
    QAudioOutput* mEarDevice;
    HDevice destinationFile;
    QFile sourcefile;
    HMicBuffer* mMicBuffer;
    HEarBuffer* mEarBuffer;
    HAECManager* mEchoManager;
    int mBitRate;
    int mFrameLenMs;

private slots:
    void handleStateChangedMic(QAudio::State newState);
    void handleStateChangedEar(QAudio::State newState);
};

#endif // HAUDIOMANAGER_H
