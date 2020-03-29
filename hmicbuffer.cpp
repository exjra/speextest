#include "hmicbuffer.h"

#include <QDateTime>
#include <QDebug>

HMicBuffer::HMicBuffer() :
    mAec(nullptr)
{
    mOutputFile.setFileName("./clear_" + QString::number(QDateTime::currentMSecsSinceEpoch()) + ".raw");
    mOutputFile.open(QIODevice::WriteOnly | QIODevice::Truncate);
}

qint64 HMicBuffer::readData(char *data, qint64 maxlen)
{
    qDebug() << "read len:" << maxlen;
    return QBuffer::readData(data, maxlen);
}

qint64 HMicBuffer::writeData(const char *data, qint64 len)
{
    mAec->init(len/2);

    mAec->onCapture(data);

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

    QBuffer::close();
}

void HMicBuffer::setAec(HAECManager *aec)
{
    mAec = aec;
}
