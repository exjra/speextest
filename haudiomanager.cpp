#include "haudiomanager.h"

#include <QDebug>
#include <QFileDialog>
#include <QSettings>

HAudioManager::HAudioManager() :
    mMicDevice(nullptr)
  , mMicBuffer(nullptr)
  , mEarBuffer(nullptr)
  , mEchoManager(nullptr)
  , mBitRate(8000)
{

}

void HAudioManager::initAudioSystem()
{

}

void HAudioManager::init()
{
    QAudioFormat format;
    format.setSampleRate(mBitRate);
    format.setChannelCount(1);
    format.setSampleSize(16);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::SignedInt);

    QAudioDeviceInfo info = QAudioDeviceInfo::defaultInputDevice();

    if (!info.isFormatSupported(format)) {
        qDebug() << "Default format not supported, trying to use the nearest.";
        format = info.nearestFormat(format);
    }

    mMicDevice = new QAudioInput(format, this);
    connect(mMicDevice, SIGNAL(stateChanged(QAudio::State)), this, SLOT(handleStateChangedMic(QAudio::State)));

//    destinationFile.setFileName("out.raw");
    if(destinationFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
        mMicDevice->start(&destinationFile);

    qDebug() << "Mic Buffer Size:" << mMicDevice->bufferSize();
}

void HAudioManager::deInit()
{
    if(mMicDevice != nullptr)
    {
        mMicDevice->stop();
        destinationFile.close();
        delete mMicDevice;
        mMicDevice = nullptr;
    }
}

void HAudioManager::play()
{
//    sourcefile.setFileName("out.raw");
    destinationFile.open(QIODevice::ReadOnly);

    QAudioFormat format;
    // Set up the format, eg.
    format.setSampleRate(mBitRate);
    format.setChannelCount(1);
    format.setSampleSize(16);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::SignedInt);

    QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
    if (!info.isFormatSupported(format)) {
        qDebug() << "Raw audio format not supported by backend, cannot play audio.";
        return;
    }

    mEarDevice = new QAudioOutput(format, this);
    connect(mEarDevice, SIGNAL(stateChanged(QAudio::State)), this, SLOT(handleStateChangedMic(QAudio::State)));
    mEarDevice->start(&destinationFile);

    qDebug() << "Ear Buffer Size:" << mEarDevice->bufferSize();
}

void HAudioManager::playRecord(QString pFile)
{
    sourcefile.setFileName(pFile);
    if(!sourcefile.exists())
        return;

    sourcefile.open(QIODevice::ReadOnly);

    QAudioFormat format;
    // Set up the format, eg.
    format.setSampleRate(mBitRate);
    format.setChannelCount(1);
    format.setSampleSize(16);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::SignedInt);

    QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
    if (!info.isFormatSupported(format)) {
        qDebug() << "Raw audio format not supported by backend, cannot play audio.";
        return;
    }

    mEarDevice = new QAudioOutput(format, this);
    connect(mEarDevice, SIGNAL(stateChanged(QAudio::State)), this, SLOT(handleStateChangedMic(QAudio::State)));
    mEarDevice->start(&sourcefile);

    qDebug() << "Ear Buffer Size:" << mEarDevice->bufferSize();
}

void HAudioManager::initWithAEC()
{
    QSettings tSettings;

    QString tInputAudioFilePath = QFileDialog::getOpenFileName(nullptr, "Select input raw audio file", tSettings.value("inputfilepath").toString());
    if(tInputAudioFilePath == nullptr)
    {
        qDebug() << "Input raw audio file must be selected!";
        return;
    }

    {
        QAudioFormat format;
        format.setSampleRate(mBitRate);
        format.setChannelCount(1);
        format.setSampleSize(16);
        format.setCodec("audio/pcm");
        format.setByteOrder(QAudioFormat::LittleEndian);
        format.setSampleType(QAudioFormat::SignedInt);

        QAudioDeviceInfo info = QAudioDeviceInfo::defaultInputDevice();

        if (!info.isFormatSupported(format)) {
            qDebug() << "Default format not supported, trying to use the nearest.";
            format = info.nearestFormat(format);
        }

        mMicDevice = new QAudioInput(format, this);

        mMicBuffer = new HMicBuffer();

        if(mMicBuffer->open(QIODevice::WriteOnly | QIODevice::Truncate))
            mMicDevice->start(mMicBuffer);
    }

    {
        QAudioFormat format;
        // Set up the format, eg.
        format.setSampleRate(mBitRate);
        format.setChannelCount(1);
        format.setSampleSize(16);
        format.setCodec("audio/pcm");
        format.setByteOrder(QAudioFormat::LittleEndian);
        format.setSampleType(QAudioFormat::SignedInt);

        QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
        if (!info.isFormatSupported(format)) {
            qDebug() << "Raw audio format not supported by backend, cannot play audio.";
            return;
        }

        mEarDevice = new QAudioOutput(format, this);

        mEarBuffer = new HEarBuffer();
        mEarBuffer->setRawFileName(tInputAudioFilePath);

        tSettings.setValue("inputfilepath", tInputAudioFilePath);

        if(mEarBuffer->open(QIODevice::ReadOnly))
            mEarDevice->start(mEarBuffer);

//        mEarDevice->setBufferSize(mMicDevice->bufferSize());
    }

    mEchoManager = new HAECManager();

    int tFilterLen = mBitRate * 300.0f/1000.0f;  //8000.0f * 300.0f/1000.0f); for 300ms!
    qDebug() << "Attention! calculated filter len is:" << tFilterLen;
    mEchoManager->setFilterLen(tFilterLen);

    mEarBuffer->setAec(mEchoManager);
    mMicBuffer->setAec(mEchoManager);
}

void HAudioManager::deInitWithAEC()
{
    if(mMicDevice != nullptr)
    {
        mMicDevice->stop();
        mMicBuffer->close();
        delete mMicDevice;
        mMicDevice = nullptr;
    }

    if(mEarDevice != nullptr)
    {
        mEarDevice->stop();
        mEarBuffer->close();

        delete mEarDevice;
        mEarDevice = nullptr;
    }
}

void HAudioManager::stopPlay()
{
    if(mEarDevice != nullptr)
    {
        mEarDevice->stop();
        sourcefile.close();
        delete mEarDevice;
        mEarDevice = nullptr;
    }
}

void HAudioManager::handleStateChangedMic(QAudio::State newState)
{
    switch (newState) {
    case QAudio::StoppedState:
        if (mMicDevice->error() != QAudio::NoError) {
            qDebug() << "(mic) Error handling";
        } else {
            qDebug() << "(mic) Finished recording";
        }
        break;

    case QAudio::ActiveState:
        qDebug() << "(mic) Started recording - read from IO device";
        break;

    default:
        qDebug() << "(mic) other cases as appropriate. State: " << newState;
        break;
    }
}

void HAudioManager::handleStateChangedEar(QAudio::State newState)
{
    switch (newState) {
    case QAudio::StoppedState:
        if (mEarDevice->error() != QAudio::NoError) {
            qDebug() << "(ear) Error handling";
        } else {
            qDebug() << "(ear) Finished recording";
        }
        break;

    case QAudio::ActiveState:
        qDebug() << "(ear) Started recording - read from IO device";
        break;

    case QAudio::IdleState:
        qDebug() << "(ear) finished";
        stopPlay();
        break;

    default:
        qDebug() << "(ear) other cases as appropriate. State: " << newState;
        break;
    }
}
