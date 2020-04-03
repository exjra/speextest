#include "haudioserver.h"

#include <QDebug>
#include <functional>

HAudioServer::HAudioServer()
{

}

void HAudioServer::init()
{
    HServerOptions::setServerAddress("harf.havelsan.com.tr"); //Optional

    HIConnectionHelper* connectionHelper = HConnectionFactory::createConnectionHelper();

    std::string errorMessage;
    connectionHelper->login("test", "MjQxNjU5Qz", errorMessage);
    if (errorMessage.empty() == false)
    {
        qDebug() << errorMessage.c_str();

        return;
    }

    connectionHelper->setOnDataReceivedFunction(std::bind(&HAudioServer::onDataReceived, this, _1, _2, _3));

    connectionHelper->setOnConnectedFunction(std::bind(&HAudioServer::onConnected, this, _1, _2));

    connectionHelper->setOnDisconnectedFunction(std::bind(&HAudioServer::onDisconnected, this));

    connectionHelper->registerClient(myself, errorMessage);
    if (errorMessage.empty() == false)
    {
        qDebug() << errorMessage.c_str();

        return;
    }

    connectionHelper->acceptConnection();

    qDebug() << "Waiting for connection.";
}

void HAudioServer::onDataReceived(short channelID , char* data, int size)
{
    qDebug() << "Data Received : " << std::string(data, size).c_str();
}

void HAudioServer::onConnected(HIConnectionHelper* connectionHelper, HClientInfo* fromClient)
{
    qDebug() << "Connected to " << fromClient->name.c_str() << ".";
    std::string msg = "Hello to " + fromClient->name + " from " + myself;
    connectionHelper->sendData(0, msg.c_str(), msg.size());
}

void HAudioServer::onDisconnected()
{
    qDebug() << "Disconnected ";
}
