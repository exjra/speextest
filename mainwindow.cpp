#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QString>
#include <QDebug>
#include <QSettings>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
  , ui(new Ui::MainWindow)
  , mAudioManager(nullptr)
{
    ui->setupUi(this);

    QSettings tSettings;

    int tVal = tSettings.value("comboBox").toInt();
    if(tVal > ui->comboBox->count() || tVal < 0)
        tVal = 0;
    ui->comboBox->setCurrentIndex(tVal);

    tVal = tSettings.value("spinBox_3").toInt();
    if(tVal > ui->spinBox_3->maximum() || tVal < ui->spinBox_3->minimum())
        tVal = -1;
    ui->spinBox_3->setValue(tVal);

    tVal = tSettings.value("spinBox").toInt();
    if(tVal > ui->spinBox->maximum() || tVal < ui->spinBox->minimum())
        tVal = -1;
    ui->spinBox->setValue(tVal);

    tVal = tSettings.value("spinBox_2").toInt();
    if(tVal > ui->spinBox_2->maximum() || tVal < ui->spinBox_2->minimum())
        tVal = -1;
    ui->spinBox_2->setValue(tVal);

    tVal = tSettings.value("spinBox_4").toInt();
    if(tVal > ui->spinBox_4->maximum() || tVal < ui->spinBox_4->minimum())
        tVal = 80;
    ui->spinBox_4->setValue(tVal);

    tVal = tSettings.value("spinBox_5").toInt();
    if(tVal > ui->spinBox_5->maximum() || tVal < ui->spinBox_5->minimum())
        tVal = 80;
    ui->spinBox_5->setValue(tVal);

    setWindowTitle("ready @11");
    qDebug() << windowTitle();
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
    mAudioManager->setSampleRate(ui->comboBox->currentText().toInt());

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
        mAudioManager->setSampleRate(ui->comboBox->currentText().toInt());
        mAudioManager->playRecord(tPath);
    }
}

void MainWindow::on_pushButton_5_clicked()
{
    if(mAudioManager == nullptr)
        return;

    QSettings tSettings;

    tSettings.setValue("comboBox", ui->comboBox->currentIndex());
    tSettings.setValue("spinBox_4", ui->spinBox_4->value());
    tSettings.setValue("spinBox_5", ui->spinBox_5->value());
    tSettings.setValue("spinBox_3", ui->spinBox_3->value());
    tSettings.setValue("spinBox", ui->spinBox->value());
    tSettings.setValue("spinBox_2", ui->spinBox_2->value());

    mAudioManager->setSampleRate(ui->comboBox->currentText().toInt());
    mAudioManager->setVolumes(ui->spinBox_4->value() * 0.01f, ui->spinBox_5->value() * 0.01f);

    int tFrameLen = ui->spinBox_3->value();
    int tFilterLen = ui->spinBox->value();
    int tInternalDelay = ui->spinBox_2->value();

    mAudioManager->initWithAEC(tFrameLen, tFilterLen, tInternalDelay);
}

void MainWindow::on_pushButton_6_clicked()
{
    if(mAudioManager == nullptr)
        return;

    mAudioManager->deInitWithAEC();
}
