#ifndef HAUDIOCLIENT_H
#define HAUDIOCLIENT_H

//connection
#include <harfconnection/hconnectionfactory.h>
#include <harfconnection/hiconnection.h>
#include <harfconnection/hiconnectionhelper.h>
#include <harfconnection/hserveroptions.h>

//stream
#include <stream/hstreamfactory.h>

#include <QBuffer>
#include <QObject>

#include "hearbuffernetwork.h"

using namespace std::placeholders;
using namespace harf::connection;

using namespace harf::stream;
using namespace harf::stream::audio;

class HAudioClient : public QObject
{
    Q_OBJECT

public:
    HAudioClient();

    void init(bool pIsServer, std::string pMyName, std::string pTargetName);

    void sendData(char* pdata, int pSize);

    void setEarBuffer(HEarBufferNetwork *earBuffer);

private:
    std::string mMyName;
    std::string mTargetName;

    HIConnectionHelper* mTargetConnection;

    void initForServer();
    void initForClient();
    void initEncoderDecoder();

    void onDataReceived(short channelID , char* data, int size);
    void onConnected(HIConnectionHelper* connectionHelper, HClientInfo* fromClient);
    void onDisconnected();
    void onConnectionTimeout();

    HEarBufferNetwork* mEarBuffer;

    HCodec mAudioCodec = HCodec::CODEC_AUDIO_MP3;

    HIAudioSendStream* mAudioSendStream = nullptr;
    HIAudioReceiveStream* mAudioReceiveStream = nullptr;

    void encodedBufferReady(char *data, int size);
    void encoderPropReceived(int rate, HAudioFormat format, int channels, HAudioLayout layout);

    void decodedBufferReady(char *data, int size);
    void decoderPropReceived(int rate, HAudioFormat format, int channels, HAudioLayout layout);

signals:
    void dataReceived(char* pData, int pSize);
};

#endif // HAUDIOSERVER_H
