#include "hearbuffer.h"

#include <QDebug>

HEarBuffer::HEarBuffer() :
    mAec(nullptr)
{

}

qint64 HEarBuffer::readData(char *data, qint64 maxlen)
{
    qDebug() << "read len:" << maxlen;
    qint64 tReadedLen = mSourceFile.read(data, maxlen);

    if(tReadedLen == maxlen)
        mAec->onPlayback(data);
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
    //    qDebug() << "read len:" << maxlen;
    //    return QBuffer::readData(data, maxlen);
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
