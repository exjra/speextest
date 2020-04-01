#ifndef HMICBUFFER_H
#define HMICBUFFER_H

#include <QDataStream>
#include <QBuffer>
#include <QFile>
#include <QElapsedTimer>

#include "haecmanager.h"

class HMicBuffer : public QBuffer
{
public:
    HMicBuffer();

    void setAec(HAECManager *aec);

    void close();

protected:
    qint64 readData(char *data, qint64 maxlen);
    qint64 writeData(const char *data, qint64 len);

private:
    HAECManager* mAec;
    QFile mOutputFile;
    QFile mOutputMicFile;

    int mByPassFrameCount;
    int mByPassFrameSize;

    QElapsedTimer mElapsed;
};

#endif // HMICBUFFER_H
