#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QString>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
  , ui(new Ui::MainWindow)
  , mAudioManager(nullptr)
{
    ui->setupUi(this);

    ui->spinBox->setValue(200);
#if defined (__ANDROID__)
    ui->spinBox->setValue(450);
#endif

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
    mAudioManager->setBitRate(ui->comboBox->currentText().toInt());

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
    if(mAudioManager == nullptr)
        return;

    QString tPath = "";

#if defined (__ANDROID__)
    tPath = "/mnt/sdcard/harf/clear.raw";
#endif

    if(tPath == "")
    {
#if defined (__ANDROID__)
        tPath = QFileDialog::getOpenFileName(this, "Select a raw audio file", "/mnt/sdcard/harf");
#else
        tPath = QFileDialog::getOpenFileName(this, "Select a raw audio file", ".");
#endif
    }

    if(tPath != "")
    {
        mAudioManager->setBitRate(ui->comboBox->currentText().toInt());
        mAudioManager->playRecord(tPath);
    }
}

void MainWindow::on_pushButton_5_clicked()
{
    if(mAudioManager == nullptr)
        return;

    mAudioManager->setBitRate(ui->comboBox->currentText().toInt());
    mAudioManager->setFrameLenMs(ui->spinBox->value());

    mAudioManager->initWithAEC();
}

void MainWindow::on_pushButton_6_clicked()
{
    if(mAudioManager == nullptr)
        return;

    mAudioManager->deInitWithAEC();
}

void MainWindow::on_pushButton_7_clicked()
{
    if(mAudioManager != nullptr)
        mAudioManager->initForInternalDelay();
}
