#include "hearbuffer.h"

#include <QDebug>

HEarBuffer::HEarBuffer() :
    mAec(nullptr)
{

}

qint64 HEarBuffer::readData(char *data, qint64 maxlen)
{
    if(mAec == nullptr)
    {
        memset(data, 0, maxlen);
        return maxlen;
    }

    QByteArray tReadedData = mSourceFile.read(maxlen);
    mMainBuffer.append(tReadedData);

    if(mMainBuffer.length() >= maxlen)
    {
        memcpy(data, mMainBuffer.data(), maxlen);
        mAec->onPlayback(data);
        mMainBuffer.remove(0, maxlen);
    }
    else
    {
        mSourceFile.close();

        mSourceFile.open(QIODevice::ReadOnly);

        mSourceFile.read(data, maxlen);

        mAec->resetAec();
        mAec->onPlayback(data);

        qDebug() << "source file re-opened";
    }

    return maxlen;
}

qint64 HEarBuffer::writeData(const char *data, qint64 len)
{
    qDebug() << "write len:" << len;
    return QBuffer::writeData(data, len);
}

void HEarBuffer::setRawFileName(const QString &rawFileName)
{
    mRawFile = rawFileName;

    mSourceFile.setFileName(mRawFile);
    mSourceFile.open(QIODevice::ReadOnly);
}

void HEarBuffer::setAec(HAECManager *aec)
{
    mAec = aec;
}
