#include "hmicbuffernetwork.h"

#include <QDateTime>
#include <QDebug>

HMicBufferNetwork::HMicBufferNetwork() :
    mAec(nullptr)
  , mByPassFrameSize(80)
  , mByPassFrameCount(0)
  #if defined(HAS_HARFSDK)
  #if !defined (__ANDROID__)
  , mClient(nullptr)
  #endif
  #endif
{

}

qint64 HMicBufferNetwork::readData(char *data, qint64 maxlen)
{
    return QBuffer::readData(data, maxlen);
}

qint64 HMicBufferNetwork::writeData(const char *data, qint64 len)
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

        char* tCleanBuffer = mAec->getCleanBuffer();

        if(tCleanBuffer != nullptr)
        {

#if defined(HAS_HARFSDK)
#if !defined (__ANDROID__)
            if(mClient)
                mClient->sendData(tCleanBuffer, tTotalBytes);
#endif
#endif
        }

        mMainBuffer.remove(0, tTotalBytes);

        //        qDebug() << "mic " << tTotalBytes << " / " << mMainBuffer.length();
    }

    return len;
}

#if defined(HAS_HARFSDK)
#if !defined (__ANDROID__)
void HMicBufferNetwork::setClient(HAudioClient *client)
{
    mClient = client;
}
#endif
#endif

void HMicBufferNetwork::close()
{        
    QBuffer::close();
}

void HMicBufferNetwork::setAec(HAECManager *aec)
{
    mAec = aec;
}
