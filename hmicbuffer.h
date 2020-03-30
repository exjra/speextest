#ifndef HMICBUFFER_H
#define HMICBUFFER_H

#include <QDataStream>
#include <QBuffer>
#include <QFile>

#include "haecmanager.h"

class HMicBuffer : public QBuffer
{
public:
    HMicBuffer();

    void setAec(HAECManager *aec);

protected:
    qint64 readData(char *data, qint64 maxlen);
    qint64 writeData(const char *data, qint64 len);

private:
    HAECManager* mAec;
    QFile mOutputFile;
    QFile mOutputMicFile;

    // QIODevice interface
public:
    void close();
};

#endif // HMICBUFFER_H
