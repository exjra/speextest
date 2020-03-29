#ifndef HDEVICE_H
#define HDEVICE_H

#include <QDataStream>
#include <QBuffer>
#include <QFile>

class HDevice : public QBuffer
{
public:
    HDevice();
    ~HDevice();

    // QIODevice interface
protected:
    qint64 readData(char *data, qint64 maxlen) override;
    qint64 writeData(const char *data, qint64 len) override;

public:
    bool open(OpenMode mode) override;
    void close() override;

private:
    QFile mFile;
};

#endif // HDEVICE_H
