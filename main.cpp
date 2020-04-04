#include "mainwindow.h"
#include <QApplication>
#include <QAudioDeviceInfo>
#include <QSettings>
#include <QDebug>
#include <QAudioRecorder>

#include "haudiomanager.h"

#if !defined (__ANDROID__)
#include "haudioclient.h"
#endif

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("Aydolabs");
    QCoreApplication::setOrganizationDomain("aydolabs.com");
    QCoreApplication::setApplicationName("SpeexTest App");

    QApplication a(argc, argv);

    HAudioManager* mAudioManager = new HAudioManager;

    MainWindow w;
    w.setAec(mAudioManager);
    w.show();

    return a.exec();
}
