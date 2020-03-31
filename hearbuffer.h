#ifndef HEARBUFFER_H
#define HEARBUFFER_H

#include <QDataStream>
#include <QBuffer>
#include <QFile>

#include "haecmanager.h"

class HEarBuffer : public QBuffer
{
public:
    HEarBuffer();

    void setAec(HAECManager *aec);

    void setRawFileName(const QString &rawFileName);

protected:
    qint64 readData(char *data, qint64 maxlen);
    qint64 writeData(const char *data, qint64 len);

private:
    HAECManager* mAec;
    QFile mSourceFile;

    QString mRawFile;

    QByteArray mMainBuffer;
};

#endif // HEARBUFFER_H
