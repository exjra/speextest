#include "hmicbuffer.h"

#include <QDateTime>
#include <QDebug>

HMicBuffer::HMicBuffer() :
    mAec(nullptr)
  , mByPassFrameSize(80)
  , mByPassFrameCount(0)
{
    QString tRootPath = "";
#if defined(__ANDROID__)
    tRootPath = "/mnt/sdcard/harf";
#else
    tRootPath = ".";
#endif
    qint64 tTimeBuff = QDateTime::currentMSecsSinceEpoch();
//    mOutputFile.setFileName(tRootPath + "/clear_" + QString::number(tTimeBuff) + ".raw");
    mOutputFile.setFileName(tRootPath + "/clear.raw");
    mOutputFile.open(QIODevice::WriteOnly | QIODevice::Truncate);

//    mOutputMicFile.setFileName(tRootPath + "/mic_" + QString::number(tTimeBuff) + ".raw");
    mOutputMicFile.setFileName(tRootPath + "/mic.raw");
    mOutputMicFile.open(QIODevice::WriteOnly | QIODevice::Truncate);
}

qint64 HMicBuffer::readData(char *data, qint64 maxlen)
{
    return QBuffer::readData(data, maxlen);
}

qint64 HMicBuffer::writeData(const char *data, qint64 len)
{
    if(mAec == nullptr) return len;

    if(!mAec->getEarReady()) return len;

    mAec->init();

    QByteArray tAry = QByteArray::fromRawData(data, len);
    mMainBuffer.append(tAry);

    int tTotalBytes = mAec->getFrameSize()*2;

    while(mMainBuffer.length() >= tTotalBytes)
    {
        mAec->onCapture(mMainBuffer.data());

        mOutputMicFile.write(mMainBuffer.data(), tTotalBytes);

        char* tCleanBuffer = mAec->getCleanBuffer();

        if(tCleanBuffer != nullptr)
            mOutputFile.write(tCleanBuffer, tTotalBytes);

        mMainBuffer.remove(0, tTotalBytes);

//        qDebug() << "mic " << tTotalBytes << " / " << mMainBuffer.length();
    }

    return len;
}

void HMicBuffer::close()
{
    if(mOutputFile.isOpen())
    {
        mOutputFile.close();

        qDebug() << "the clean file finalized";
    }

    if(mOutputMicFile.isOpen())
    {
        mOutputMicFile.close();

        qDebug() << "the mic file finalized";
    }

    QBuffer::close();
}

void HMicBuffer::setAec(HAECManager *aec)
{
    mAec = aec;
}
