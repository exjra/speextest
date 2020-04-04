#include "haudioclient.h"

#include <QDebug>
#include <functional>

HAudioClient::HAudioClient() :
    mTargetConnection(nullptr)
{

}

void HAudioClient::init(bool pIsServer, std::string pMyName, std::string pTargetName)
{
    mMyName = pMyName;
    mTargetName = pTargetName;

    if(pIsServer)
    {
        if(mMyName != "")
            initForServer();
        else
            qDebug() << "Client name(" << QString::fromStdString(mMyName) << " must be set!";
    }
    else
    {
        if(mMyName != "" && mTargetName != "")
            initForClient();
        else
            qDebug() << "Client name(" << QString::fromStdString(mMyName) << ") or Target Name (" << QString::fromStdString(mTargetName) << ") must be set!";
    }
}

void HAudioClient::sendData(char *pdata, int pSize)
{
    if(mTargetConnection == nullptr)
        return;

    mTargetConnection->sendData(0, pdata, pSize);
    qDebug() << "data sent";
}

void HAudioClient::initForServer()
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

    connectionHelper->setOnDataReceivedFunction(std::bind(&HAudioClient::onDataReceived, this, _1, _2, _3));
    connectionHelper->setOnConnectedFunction(std::bind(&HAudioClient::onConnected, this, _1, _2));
    connectionHelper->setOnDisconnectedFunction(std::bind(&HAudioClient::onDisconnected, this));

    connectionHelper->registerClient(mMyName, errorMessage);
    if (errorMessage.empty() == false)
    {
        qDebug() << errorMessage.c_str();

        return;
    }

    connectionHelper->acceptConnection();

    qDebug() << "Waiting for clients.";
}

void HAudioClient::initForClient()
{
    HServerOptions::setServerAddress("harf.havelsan.com.tr");

    HIConnectionHelper* connectionHelper = HConnectionFactory::createConnectionHelper();

    std::string errorMessage;
    connectionHelper->login("test", "MjQxNjU5Qz", errorMessage);
    if (errorMessage.empty() == false)
    {
        qDebug() << errorMessage.c_str();

        return;
    }

    connectionHelper->setOnDataReceivedFunction(std::bind(&HAudioClient::onDataReceived, this, _1, _2, _3));
    connectionHelper->setOnConnectedFunction(std::bind(&HAudioClient::onConnected, this, _1, _2));
    connectionHelper->setOnDisconnectedFunction(std::bind(&HAudioClient::onDisconnected, this));
    connectionHelper->setOnConnectionTimeoutFunction(std::bind(&HAudioClient::onConnectionTimeout, this));

    connectionHelper->registerClient(mMyName, errorMessage);
    if (errorMessage.empty() == false)
    {
        qDebug() << errorMessage.c_str();

        return;
    }

    connectionHelper->startConnectionTo(mTargetName,errorMessage);
    if (errorMessage.empty() == false)
    {
        qDebug() << errorMessage.c_str();

        return;
    }

    qDebug() << "Waiting for connecting to server.";
}

void HAudioClient::onDataReceived(short channelID , char* data, int size)
{
    if(mEarBuffer == nullptr)
        return;

    mEarBuffer->write(data, size);
//    dataReceived(data, size);
//    qDebug() << "Data Received : " << std::string(data, size).c_str();
}

void HAudioClient::onConnected(HIConnectionHelper* connectionHelper, HClientInfo* fromClient)
{
    mTargetConnection = connectionHelper;
    qDebug() << "Connected to " << fromClient->name.c_str() << ".";
//    std::string msg = "Hello to " + fromClient->name + " from " + mMyName;
//    connectionHelper->sendData(0, msg.c_str(), msg.size());
}

void HAudioClient::onDisconnected()
{
    qDebug() << "Disconnected ";
}

void HAudioClient::onConnectionTimeout()
{
    qDebug() << "Connection timeout";
}

void HAudioClient::setEarBuffer(QBuffer *earBuffer)
{
    mEarBuffer = earBuffer;
}
