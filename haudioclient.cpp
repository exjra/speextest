#include "haudioclient.h"

#include <QDebug>
#include <functional>

HAudioClient::HAudioClient() :
    mTargetConnection(nullptr)
  , mEarBuffer(nullptr)
{

}

void HAudioClient::init(bool pIsServer, std::string pMyName, std::string pTargetName)
{
    mMyName = pMyName;
    mTargetName = pTargetName;

    if(pIsServer)
    {
        if(mMyName != "")
        {
            initEncoderDecoder();
            initForServer();
        }
        else
            qDebug() << "Client name(" << QString::fromStdString(mMyName) << " must be set!";
    }
    else
    {
        if(mMyName != "" && mTargetName != "")
        {
            initEncoderDecoder();
            initForClient();
        }
        else
            qDebug() << "Client name(" << QString::fromStdString(mMyName) << ") or Target Name (" << QString::fromStdString(mTargetName) << ") must be set!";
    }
}

void HAudioClient::sendData(char *pdata, int pSize)
{
    if(mTargetConnection == nullptr)
        return;

    if (mAudioSendStream != nullptr)
    {
        if(HStreamState::STATE_PLAYING == mAudioSendStream->getStreamState() || HStreamState::STATE_PAUSED == mAudioSendStream->getStreamState())
            mAudioSendStream->feedStream(pdata, pSize);
    }
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

void HAudioClient::initEncoderDecoder()
{
    mAudioSendStream = HStreamFactory::createAudioSendStream();
    mAudioSendStream->setInput(HInput::INPUT_FROM_BUFFER);
    mAudioSendStream->setOutput(HOutput::OUTPUT_TO_BUFFER);
    mAudioSendStream->setPayloadEnabled(true);
    mAudioSendStream->setStreamDataFunction(std::bind(&HAudioClient::encodedBufferReady, this, _1, _2));
    mAudioSendStream->setAudioPropertyFunction(std::bind(&HAudioClient::encoderPropReceived, this, _1, _2, _3, _4));
    mAudioSendStream->initialize(mAudioCodec);
    mAudioSendStream->play();

    mAudioReceiveStream = HStreamFactory::createAudioReceiveStream();
    mAudioReceiveStream->setInput(HInput::INPUT_FROM_BUFFER);
    mAudioReceiveStream->setOutput(HOutput::OUTPUT_TO_BUFFER);
    mAudioReceiveStream->setPayloadEnabled(true);
    mAudioReceiveStream->setRawDataFunction(std::bind(&HAudioClient::decodedBufferReady, this, _1, _2));
    mAudioReceiveStream->setAudioPropertyFunction(std::bind(&HAudioClient::decoderPropReceived, this, _1, _2, _3, _4));
    mAudioReceiveStream->initialize(mAudioCodec);
    mAudioReceiveStream->play();
}

void HAudioClient::encodedBufferReady(char* data, int size)
{
    if(mTargetConnection != nullptr)
        mTargetConnection->sendData(0, data, size);
}

void HAudioClient::encoderPropReceived(int rate, HAudioFormat format, int channels, HAudioLayout layout)
{
    qDebug() << "Encoder Audio Props are, Rate: " << rate << " , Format: " << (int)format << " ,Channels: " << channels << " ,Layout: " << (int)layout;
}

void HAudioClient::decodedBufferReady(char *data, int size)
{
    if(mEarBuffer == nullptr)
        return;

    mEarBuffer->write(data, size);
}

void HAudioClient::decoderPropReceived(int rate, HAudioFormat format, int channels, HAudioLayout layout)
{
    qDebug() << "Decoder Audio Props are, Rate: " << rate << " , Format: " << (int)format << " ,Channels: " << channels << " ,Layout: " << (int)layout;
}

void HAudioClient::onDataReceived(short channelID , char* data, int size)
{
    if (mAudioReceiveStream != nullptr)
    {
        if(HStreamState::STATE_PLAYING == mAudioReceiveStream->getStreamState() || HStreamState::STATE_PAUSED == mAudioReceiveStream->getStreamState())
            mAudioReceiveStream->feedStream(data, size);
    }
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

void HAudioClient::setEarBuffer(HEarBufferNetwork *earBuffer)
{
    mEarBuffer = earBuffer;
}
