#include "hdevice.h"

#include <QDebug>
#include <QDateTime>

HDevice::HDevice()
{

}

HDevice::~HDevice()
{

}

qint64 HDevice::readData(char *data, qint64 maxlen)
{
    qDebug() << "read len:" << maxlen;
    qint64 tRet = QBuffer::readData(data, maxlen);

    return tRet;
}

qint64 HDevice::writeData(const char *data, qint64 len)
{
    qDebug() << "write len:" << len;

    qint64 tRet = QBuffer::writeData(data, len);
    if(mFile.isOpen() && mFile.isWritable())
        mFile.write(data, tRet);

    return tRet;
}

bool HDevice::open(QIODevice::OpenMode mode)
{
    mFile.setFileName("./record_" + QString::number(QDateTime::currentMSecsSinceEpoch()) + ".raw");
    mFile.open(QIODevice::WriteOnly | QIODevice::Truncate);

    return QBuffer::open(mode);
}

void HDevice::close()
{
    if(mFile.isOpen())
        mFile.close();

    QBuffer::close();
}
