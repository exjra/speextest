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
    else if(!mAec->initialized())
    {
        memset(data, 0, maxlen);
        return maxlen;
    }

    QByteArray tReadedData = mSourceFile.read(maxlen);
    mMainBuffer.append(tReadedData);

    if(mMainBuffer.length() >= maxlen)
        memcpy(data, mMainBuffer.data(), maxlen);

    int tCount = 0;
    while(true)
    {
        if(mMainBuffer.length() >= mAec->getFrameSize()*2)
        {
//            memcpy(data, mMainBuffer.data(), maxlen);
            mAec->onPlayback(data);
            mMainBuffer.remove(0, mAec->getFrameSize()*2);
            qDebug() << "Push ear:" << QString::number(tCount) << " Len: " << maxlen;
            tCount++;
        }
        else
        {
//            mAec->resetAec();
            //        mAec->onPlayback(data);

            break;
        }
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
