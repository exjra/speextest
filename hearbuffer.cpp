#include "hearbuffer.h"

#include <QDebug>

HEarBuffer::HEarBuffer() :
    mAec(nullptr)
{

}

//qint64 HEarBuffer::readData(char *data, qint64 maxlen)
//{
//    if(mAec == nullptr)
//    {
//        memset(data, 0, maxlen);
//        return maxlen;
//    }
//    else if(!mAec->initialized())
//    {
//        memset(data, 0, maxlen);
//        return maxlen;
//    }

//    mSourceFile.read(data, mAec->getFrameSize()*2);
//    mAec->onPlayback(data);

//    qDebug() << "ear:" << mAec->getFrameSize()*2 << " / " << maxlen;

//    return maxlen;
//}


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

//    if(mFirstTime)
//    {
//        if(mMainBuffer)
//        mFirstTime = false;

//    }

    int tCount = 0;
    while(true)
    {
        if(mMainBuffer.length() >= mAec->getFrameSize()*2)
        {
            mAec->onPlayback(mMainBuffer.data());
            mMainBuffer.remove(0, mAec->getFrameSize()*2);
            tCount++;
        }
        else
            break;
    }

    qDebug() << "ear:" << mAec->getFrameSize()*2 << " / " << maxlen;

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
