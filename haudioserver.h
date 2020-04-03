#ifndef HAUDIOSERVER_H
#define HAUDIOSERVER_H

#include <harfconnection/hconnectionfactory.h>
#include <harfconnection/hiconnection.h>
#include <harfconnection/hiconnectionhelper.h>
#include <harfconnection/hserveroptions.h>

using namespace std::placeholders;
using namespace harf::connection;

class HAudioServer
{
public:
    HAudioServer();

    void init();

private:
    std::string myself = "Peer1";

    void onDataReceived(short channelID , char* data, int size);
    void onConnected(HIConnectionHelper* connectionHelper, HClientInfo* fromClient);
    void onDisconnected();
};

#endif // HAUDIOSERVER_H
