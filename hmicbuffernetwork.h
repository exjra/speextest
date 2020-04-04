#ifndef HMICBUFFERNETWORK_H
#define HMICBUFFERNETWORK_H

#include <QDataStream>
#include <QBuffer>
#include <QFile>
#include <QElapsedTimer>

#include "haecmanager.h"

#if defined(HAS_HARFSDK)
#if !defined (__ANDROID__)
#include "haudioclient.h"
#endif
#endif

class HMicBufferNetwork : public QBuffer
{
public:
    HMicBufferNetwork();

    void setAec(HAECManager *aec);

    void close();

#if defined(HAS_HARFSDK)
#if !defined (__ANDROID__)
    void setClient(HAudioClient *client);
#endif
#endif

protected:
    qint64 readData(char *data, qint64 maxlen);
    qint64 writeData(const char *data, qint64 len);

private:
    HAECManager* mAec;

    int mByPassFrameCount;
    int mByPassFrameSize;

    QElapsedTimer mElapsed;
    QByteArray mMainBuffer;

#if defined(HAS_HARFSDK)
#if !defined (__ANDROID__)
    HAudioClient* mClient;
#endif
#endif
};

#endif // HMICBUFFER_H
