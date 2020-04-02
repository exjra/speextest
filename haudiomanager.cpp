#include "haudiomanager.h"

#include <QDebug>
#include <QFileDialog>
#include <QSettings>
#include <QThread>

HAudioManager::HAudioManager() :
    mMicDevice(nullptr)
  , mMicBuffer(nullptr)
  , mEarBuffer(nullptr)
  , mEchoManager(nullptr)
  , mSampleRate(8000)
  , mMicVolume(1.0)
  , mSpeakerVolume(1.0)
{

}

void HAudioManager::initAudioSystem()
{

}

void HAudioManager::init()
{
    QAudioFormat format;
    format.setSampleRate(mSampleRate);
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
    format.setSampleRate(mSampleRate);
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
    format.setSampleRate(mSampleRate);
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
    mEarDevice->setBufferSize((mSampleRate/10)*2);
    mEarDevice->setVolume(0.7);
    connect(mEarDevice, SIGNAL(stateChanged(QAudio::State)), this, SLOT(handleStateChangedMic(QAudio::State)));
    mEarDevice->start(&sourcefile);

    qDebug() << "Ear Buffer Size:" << mEarDevice->bufferSize();
}

void HAudioManager::initWithAEC(int pFrameLenMs, int pFilterLenMs, int pInternalDelayMs)
{
    mEchoManager = new HAECManager();

    mEchoManager->setFrameSizeMs(pFrameLenMs);
    mEchoManager->setFilterLenMs(pFilterLenMs);
    mEchoManager->setInternalDelayLenMs(pInternalDelayMs);
    mEchoManager->setSamplingRate(mSampleRate);

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
        format.setSampleRate(mSampleRate);
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

        QThread* tThread = new QThread();
        mMicDevice = new QAudioInput(format, tThread);
        mMicDevice->setBufferSize(mEchoManager->calculateAudioBufferLength());
        mMicDevice->setVolume(mMicVolume);
        connect(mMicDevice, SIGNAL(stateChanged(QAudio::State)), this, SLOT(handleStateChangedMic(QAudio::State)));

        tThread->start();

        mMicBuffer = new HMicBuffer();

        if(mMicBuffer->open(QIODevice::WriteOnly | QIODevice::Truncate))
            mMicDevice->start(mMicBuffer);
    }

    {
        QAudioFormat format;
        // Set up the format, eg.
        format.setSampleRate(mSampleRate);
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

        QThread* tThread = new QThread();
        mEarDevice = new QAudioOutput(format, tThread);
        mEarDevice->setBufferSize(mEchoManager->calculateAudioBufferLength());
        mEarDevice->setVolume(mSpeakerVolume);
        connect(mEarDevice, SIGNAL(stateChanged(QAudio::State)), this, SLOT(handleStateChangedEar(QAudio::State)));

        tThread->start();

        mEarBuffer = new HEarBuffer();
        mEarBuffer->setRawFileName(tInputAudioFilePath);

        tSettings.setValue("inputfilepath", tInputAudioFilePath);

        if(mEarBuffer->open(QIODevice::ReadOnly))
            mEarDevice->start(mEarBuffer);
    }

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

void HAudioManager::setSampleRate(int bitRate)
{
    mSampleRate = bitRate;
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

void HAudioManager::initForInternalDelay()
{

}

void HAudioManager::setVolumes(float pMic, float pSpeaker)
{
    mMicVolume = pMic;
    mSpeakerVolume = pSpeaker;
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
//        tSender->setBufferSize(4410);
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
//        tSender->setBufferSize(4410);
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
