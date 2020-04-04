#ifndef HAUDIOCLIENT_H
#define HAUDIOCLIENT_H

#include <harfconnection/hconnectionfactory.h>
#include <harfconnection/hiconnection.h>
#include <harfconnection/hiconnectionhelper.h>
#include <harfconnection/hserveroptions.h>

#include <QBuffer>
#include <QObject>

using namespace std::placeholders;
using namespace harf::connection;

class HAudioClient : public QObject
{
    Q_OBJECT

public:
    HAudioClient();

    void init(bool pIsServer, std::string pMyName, std::string pTargetName);

    void sendData(char* pdata, int pSize);

    void setEarBuffer(QBuffer *earBuffer);

private:
    std::string mMyName;
    std::string mTargetName;

    HIConnectionHelper* mTargetConnection;

    void initForServer();
    void initForClient();
    void onDataReceived(short channelID , char* data, int size);
    void onConnected(HIConnectionHelper* connectionHelper, HClientInfo* fromClient);
    void onDisconnected();
    void onConnectionTimeout();

    QBuffer* mEarBuffer;

signals:
    void dataReceived(char* pData, int pSize);
};

#endif // HAUDIOSERVER_H
