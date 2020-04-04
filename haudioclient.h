#ifndef HAUDIOCLIENT_H
#define HAUDIOCLIENT_H

#include <harfconnection/hconnectionfactory.h>
#include <harfconnection/hiconnection.h>
#include <harfconnection/hiconnectionhelper.h>
#include <harfconnection/hserveroptions.h>

using namespace std::placeholders;
using namespace harf::connection;

class HAudioClient
{
public:
    HAudioClient();

    void init(bool pIsServer, std::string pMyName, std::string pTargetName);

private:
    std::string mMyName;
    std::string mTargetName;

    void initForServer();
    void initForClient();
    void onDataReceived(short channelID , char* data, int size);
    void onConnected(HIConnectionHelper* connectionHelper, HClientInfo* fromClient);
    void onDisconnected();
    void onConnectionTimeout();
};

#endif // HAUDIOSERVER_H
