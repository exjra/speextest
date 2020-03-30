#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
  , ui(new Ui::MainWindow)
  , mAudioManager(nullptr)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setAec(HAudioManager *aec)
{
    mAudioManager = aec;
}

void MainWindow::on_pushButton_clicked()
{
    if(mAudioManager == nullptr)
        return;

    mAudioManager->init();
}

void MainWindow::on_pushButton_2_clicked()
{
    if(mAudioManager == nullptr)
        return;

    mAudioManager->deInit();
}

void MainWindow::on_pushButton_3_clicked()
{
    mAudioManager->play();
}

void MainWindow::on_pushButton_4_clicked()
{
    QString tPath = "";
#if defined (__ANDROID__)
    tPath = QFileDialog::getOpenFileName(this, "Select a raw audio file", "/mnt/sdcard/harf/aec/");
#else
    tPath = QFileDialog::getOpenFileName(this, "Select a raw audio file", ".");
#endif
    if(tPath != "")
        mAudioManager->playRecord(tPath);
}

void MainWindow::on_pushButton_5_clicked()
{
    if(mAudioManager == nullptr)
        return;

    mAudioManager->initWithAEC();
}

void MainWindow::on_pushButton_6_clicked()
{
    if(mAudioManager == nullptr)
        return;

    mAudioManager->deInitWithAEC();
}
