#include "mainwindow.h"
#include <QApplication>

#include "haudiomanager.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    HAudioManager* mEchoCanceller = new HAudioManager;

    MainWindow w;
    w.setAec(mEchoCanceller);
    w.show();

    return a.exec();
}
