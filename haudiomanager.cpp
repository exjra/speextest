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
  , mFrameLenMs(300)
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
    mEarDevice->setBufferSize(mBitRate*2/10);
    mEarDevice->setVolume(0.7);
    connect(mEarDevice, SIGNAL(stateChanged(QAudio::State)), this, SLOT(handleStateChangedMic(QAudio::State)));
    mEarDevice->start(&sourcefile);

    qDebug() << "Ear Buffer Size:" << mEarDevice->bufferSize();
}

void HAudioManager::initWithAEC()
{
    mEchoManager = new HAECManager();
    int tFilterLen = mBitRate * ((float) mFrameLenMs) / 1000.0f;
    mEchoManager->setFilterLen(tFilterLen);
    mEchoManager->setSamplingRate(mBitRate);

    QSettings tSettings;

    QString tInputAudioFilePath;

#if defined (__ANDROID__)
    tInputAudioFilePath = "/mnt/sdcard/harf/record.raw";
#endif

    if(tInputAudioFilePath == "")
        tInputAudioFilePath = QFileDialog::getOpenFileName(nullptr, "Select input raw audio file", tSettings.value("inputfilepath").toString());

    if(tInputAudioFilePath == "")
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
        mMicDevice->setBufferSize(mBitRate*2/20);
        mMicDevice->setVolume(0.8);
        connect(mMicDevice, SIGNAL(stateChanged(QAudio::State)), this, SLOT(handleStateChangedMic(QAudio::State)));

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
        mEarDevice->setBufferSize(mBitRate*2/20);
        mEarDevice->setVolume(0.7);
        connect(mEarDevice, SIGNAL(stateChanged(QAudio::State)), this, SLOT(handleStateChangedEar(QAudio::State)));

        mEarBuffer = new HEarBuffer();
        mEarBuffer->setRawFileName(tInputAudioFilePath);

        tSettings.setValue("inputfilepath", tInputAudioFilePath);

        if(mEarBuffer->open(QIODevice::ReadOnly))
            mEarDevice->start(mEarBuffer);
    }

    mEarBuffer->setAec(mEchoManager);
    mMicBuffer->setAec(mEchoManager);

    qDebug() << "ready @3";
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

void HAudioManager::setBitRate(int bitRate)
{
    mBitRate = bitRate;
}

void HAudioManager::setFrameLenMs(int frameLenMs)
{
    mFrameLenMs = frameLenMs;
}

void HAudioManager::resetAudioDevices()
{
//    if(mMicDevice != nullptr)
//    {
//        mMicDevice->reset();
//        mMicDevice->resume();
//    }

//    if(mEarDevice != nullptr)
//    {
//        mEarDevice->reset();
//        mEarDevice->resume();
//    }
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
    QAudioInput* tSender = (QAudioInput*) sender();
    if(tSender == nullptr)
        return;

    switch (newState) {
    case QAudio::StoppedState:
        if (tSender->error() != QAudio::NoError) {
            qDebug() << "(mic) Error handling";
        } else {
            qDebug() << "(mic) Finished recording";
        }
        break;

    case QAudio::ActiveState:
        tSender->setBufferSize(4410);
        qDebug() << "2 (mic) Started recording - read from IO device";

        qDebug() << "Input Format -----------------------------";
        qDebug() << "Sample Type:" << tSender->format().sampleType();
        qDebug() << "Sample Rate:" << tSender->format().sampleRate();
        qDebug() << "Sample Size:" << tSender->format().sampleSize();
        qDebug() << "Channels:" << tSender->format().channelCount();
        qDebug() << "Codec:" << tSender->format().codec();
        qDebug() << "Byte Order:" << tSender->format().byteOrder();
        qDebug() << "Periode Size:" << tSender->periodSize();
        qDebug() << "Volume:" << tSender->volume();
        qDebug() << "Buffer size:" << tSender->bufferSize();
        qDebug() << "------------------------------------------";

        break;

    default:
        qDebug() << "(mic) other cases as appropriate. State: " << newState;
        sourcefile.close();
        break;
    }
}

void HAudioManager::handleStateChangedEar(QAudio::State newState)
{
    QAudioOutput* tSender = (QAudioOutput*) sender();
    if(tSender == nullptr)
        return;

    switch (newState) {
    case QAudio::StoppedState:
        if (tSender->error() != QAudio::NoError) {
            qDebug() << "(ear) Error handling";
        } else {
            qDebug() << "(ear) Finished recording";
        }
        break;

    case QAudio::ActiveState:
        tSender->setBufferSize(4410);
        qDebug() << "2 (ear) Started recording - read from IO device";

        qDebug() << "Output Format ----------------------------";
        qDebug() << "Sample Type:" << tSender->format().sampleType();
        qDebug() << "Sample Rate:" << tSender->format().sampleRate();
        qDebug() << "Sample Size:" << tSender->format().sampleSize();
        qDebug() << "Channels:" << tSender->format().channelCount();
        qDebug() << "Codec:" << tSender->format().codec();
        qDebug() << "Byte Order:" << tSender->format().byteOrder();
        qDebug() << "Periode Size:" << tSender->periodSize();
        qDebug() << "Volume:" << tSender->volume();
        qDebug() << "Buffer size:" << tSender->bufferSize();
        qDebug() << "------------------------------------------";

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
