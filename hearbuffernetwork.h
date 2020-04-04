#ifndef HEARBUFFERNETWORK_H
#define HEARBUFFERNETWORK_H

#include <QDataStream>
#include <QBuffer>
#include <QFile>
#include <QMutex>
#include <QMutexLocker>
#include <QElapsedTimer>

#include "haecmanager.h"

class HEarBufferNetwork : public QBuffer
{
public:
    HEarBufferNetwork();

    void setAec(HAECManager *aec);

protected:
    qint64 readData(char *data, qint64 maxlen);
    qint64 writeData(const char *data, qint64 len);

private:
    HAECManager* mAec;

    QString mRawFile;

    QByteArray mMainBuffer;
    QByteArray mDelayBuffer;
    char* mDelayBuff;

    QByteArray mDataBuffer;
    QElapsedTimer mElapsed;

    bool mFirstTime;

    QMutex mMutex;
};

#endif // HEARBUFFER_H
