#include "hmicbuffer.h"

#include <QDateTime>
#include <QDebug>

HMicBuffer::HMicBuffer() :
    mAec(nullptr)
{
    QString tRootPath = "";
#if defined(__ANDROID__)
    tRootPath = "/mnt/sdcard/harf";
#else
    tRootPath = ".";
#endif
    qint64 tTimeBuff = QDateTime::currentMSecsSinceEpoch();
    mOutputFile.setFileName(tRootPath + "/clear_" + QString::number(tTimeBuff) + ".raw");
    mOutputFile.open(QIODevice::WriteOnly | QIODevice::Truncate);

    mOutputMicFile.setFileName(tRootPath + "/mic_" + QString::number(tTimeBuff) + ".raw");
    mOutputMicFile.open(QIODevice::WriteOnly | QIODevice::Truncate);
}

qint64 HMicBuffer::readData(char *data, qint64 maxlen)
{
    qDebug() << "read len:" << maxlen;
    return QBuffer::readData(data, maxlen);
}

qint64 HMicBuffer::writeData(const char *data, qint64 len)
{
    qDebug() << "write len:" << len;
    mAec->init(len/2);

    mAec->onCapture(data);

    mOutputMicFile.write(data, mAec->getFrameSize()*2);

    char* tCleanBuffer = mAec->getCleanBuffer();

    if(tCleanBuffer != nullptr)
        mOutputFile.write(tCleanBuffer, mAec->getFrameSize()*2);

    return len;
//    qDebug() << "write len:" << len;
    //    return QBuffer::writeData(data, len);
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
