#include "mainwindow.h"
#include <QApplication>
#include <QSettings>

#include "haudiomanager.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("Havelsan");
    QCoreApplication::setOrganizationDomain("harf.havelsan.com.tr");
    QCoreApplication::setApplicationName("SpeexTest App");

    QApplication a(argc, argv);

    HAudioManager* mEchoCanceller = new HAudioManager;

    MainWindow w;
    w.setAec(mEchoCanceller);
    w.show();

    return a.exec();
}