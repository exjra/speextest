#include "hearbuffernetwork.h"

#include <QDebug>

HEarBufferNetwork::HEarBufferNetwork() :
    mAec(nullptr)
  , mFirstTime(true)
{

}

qint64 HEarBufferNetwork::readData(char *data, qint64 maxlen)
{
    if(mAec == nullptr)
    {
        memset(data, 0, maxlen);
        return maxlen;
    }
    else if(!mAec->initialized())
    {
        if(!mAec->getEarReady())
            mAec->setEarReady(true);

        memset(data, 0, maxlen);
        return maxlen;
    }

    if(!mAec->getEarReady())
        mAec->setEarReady(true);

    if(mDataBuffer.length() < maxlen)
    {
        mAec->resetAec();

        memset(data, 0, maxlen);
        return maxlen;
    }

    QByteArray tReadedData = mDataBuffer.left(maxlen);
    mDataBuffer.remove(0, maxlen);

    memcpy(data, tReadedData.data(), maxlen);
    mMainBuffer.append(tReadedData);

    if(mFirstTime)
    {
        mFirstTime = false;
        int tDelaySize = mAec->getInternalDelayLen();

        mDelayBuff = new char[tDelaySize];
        memset(mDelayBuff, 0, tDelaySize);
        mDelayBuffer = QByteArray::fromRawData(mDelayBuff, tDelaySize);

        qDebug() << "Delay buffer initialized: " << mDelayBuffer.length() << tDelaySize;
    }

    int tSeek = 0;
    int tTotalBytes = mAec->getFrameSize()*2;

    while(mMainBuffer.length() + mDelayBuffer.length() >= tTotalBytes)
    {
//        memcpy(data + tSeek, mMainBuffer.data(), tTotalBytes); //push to speaker buffer
//        tSeek += tTotalBytes;

        if(mDelayBuffer.length() >= mAec->getFrameSize()*2)
        {
            mAec->onPlayback(mDelayBuffer.data()); //push to aec
            mDelayBuffer.remove(0, mAec->getFrameSize()*2); //decrement main buffer

//            qDebug() << "from delay buff:" << tTotalBytes << " delay: " << mDelayBuffer.length() << " main: " << mMainBuffer.length();
        }
        else
        {
            if(mDelayBuffer.length() > 0)
            {
                int tCurrBytes = 0;
                QByteArray tTotal;

                tCurrBytes += mDelayBuffer.length();
                tTotal.append(mDelayBuffer);
                mDelayBuffer.clear();

                tTotal.append(mMainBuffer.left(tTotalBytes - tCurrBytes));
                mMainBuffer.remove(0, tTotalBytes - tCurrBytes);

                mAec->onPlayback(tTotal.data());

//                qDebug() << "from delay buff + main buff:" << tTotal.length() << " delay: " << mDelayBuffer.length() << " main: " << mMainBuffer.length();
            }
            else
            {
                mAec->onPlayback(mMainBuffer.data()); //push to aec
                mMainBuffer.remove(0, tTotalBytes); //decrement main buffer

//                qDebug() << "from main buff:" << tTotalBytes << " delay: " << mDelayBuffer.length() << " main: " << mMainBuffer.length();
            }
        }
    }


    return maxlen;
}


qint64 HEarBufferNetwork::writeData(const char *data, qint64 len)
{
    mDataBuffer.append(data, len);
    return len;
}

void HEarBufferNetwork::setAec(HAECManager *aec)
{
    mAec = aec;
}
