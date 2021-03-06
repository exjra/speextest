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
  #if defined(HAS_HARFSDK)
  #if !defined (__ANDROID__)
  , mClient(nullptr)
  #endif
  #endif
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

    setWindowTitle("ready @16");
    qDebug() << windowTitle();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setAec(HAudioManager *aec)
{
    mAudioManager = aec;
    connect(mAudioManager, &HAudioManager::onSpeechState, this, &MainWindow::onSpeechState);
    connect(mAudioManager, &HAudioManager::onTimeDiff, this, &MainWindow::onTimeDiff);
}

void MainWindow::on_pushButton_clicked()
{
    mAudioManager->setSampleRate(ui->comboBox->currentText().toInt());

    if(mAudioManager == nullptr)
        return;

    mAudioManager->init();

    ui->pushButton->setEnabled(false);
    ui->pushButton_2->setEnabled(true);
    ui->pushButton_5->setEnabled(false);
    ui->pushButton_6->setEnabled(false);
    ui->groupBox_2->setEnabled(false);
    ui->groupBox_3->setEnabled(false);
}

void MainWindow::on_pushButton_2_clicked()
{
    if(mAudioManager == nullptr)
        return;

    mAudioManager->deInit();

    ui->pushButton->setEnabled(true);
    ui->pushButton_2->setEnabled(true);
    ui->pushButton_5->setEnabled(true);
    ui->pushButton_6->setEnabled(true);
    ui->groupBox_2->setEnabled(true);
    ui->groupBox_3->setEnabled(true);
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

        ui->groupBox->setEnabled(false);
        ui->pushButton_4->setEnabled(false);
        ui->pushButton_7->setEnabled(true);
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

    ui->pushButton->setEnabled(false);
    ui->pushButton_2->setEnabled(false);
    ui->pushButton_5->setEnabled(false);
    ui->pushButton_6->setEnabled(true);
    ui->groupBox_2->setEnabled(false);
    ui->groupBox_3->setEnabled(false);
}

void MainWindow::on_pushButton_6_clicked()
{
    if(mAudioManager == nullptr)
        return;

    mAudioManager->deInitWithAEC();

    ui->pushButton->setEnabled(true);
    ui->pushButton_2->setEnabled(true);
    ui->pushButton_5->setEnabled(true);
    ui->pushButton_6->setEnabled(true);
    ui->groupBox_2->setEnabled(true);
    ui->groupBox_3->setEnabled(true);

    setStyleSheet("");
}

void MainWindow::onSpeechState(bool pSpeech)
{
    if(pSpeech)
        setStyleSheet("background-color: rgb(0, 255, 0);");
    else
        setStyleSheet("");
}

void MainWindow::onTimeDiff(int pdiff)
{
    qDebug() << "Time difference between first speaker buffer and first mic buffer : " << QString::number(pdiff);
}

void MainWindow::on_pushButton_7_clicked()
{
    if(mAudioManager == nullptr)
        return;

    mAudioManager->stopPlay();

    ui->groupBox->setEnabled(true);
    ui->pushButton_4->setEnabled(true);
    ui->pushButton_7->setEnabled(true);
}
#if defined(HAS_HARFSDK)
#if !defined (__ANDROID__)
void MainWindow::setClient(HAudioClient *client)
{
    mClient = client;
}
#endif
#endif

void MainWindow::on_pushButton_3_clicked()
{
#if defined(HAS_HARFSDK)
#if !defined (__ANDROID__)

    if(mClient == nullptr)
        return;

    if(ui->lineEdit->text() == "")
    {
        qDebug() << "You must set a name for network server initialization";
        return;
    }

    mClient->init(true, ui->lineEdit->text().toStdString(), "");

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

    mAudioManager->initForNetwork(mClient, tFrameLen, tFilterLen, tInternalDelay, ui->checkBox->isChecked(), ui->checkBox_2->isChecked());

    ui->pushButton->setEnabled(false);
    ui->pushButton_2->setEnabled(false);
    ui->pushButton_5->setEnabled(false);
    ui->pushButton_6->setEnabled(true);
    ui->groupBox_2->setEnabled(false);
    ui->groupBox_3->setEnabled(false);

#endif
#endif
}

void MainWindow::on_pushButton_8_clicked()
{
#if defined(HAS_HARFSDK)
#if !defined (__ANDROID__)

    if(mClient == nullptr)
        return;

    if(ui->lineEdit->text() == "" || ui->lineEdit_2->text() == "")
    {
        qDebug() << "You must set your name and target name for network client initialization";return;
        return;
    }

    mClient->init(false, ui->lineEdit->text().toStdString(), ui->lineEdit_2->text().toStdString());

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

    mAudioManager->initForNetwork(mClient, tFrameLen, tFilterLen, tInternalDelay, ui->checkBox->isChecked(), ui->checkBox_2->isChecked());

    ui->pushButton->setEnabled(false);
    ui->pushButton_2->setEnabled(false);
    ui->pushButton_5->setEnabled(false);
    ui->pushButton_6->setEnabled(true);
    ui->groupBox_2->setEnabled(false);
    ui->groupBox_3->setEnabled(false);

#endif
#endif
}
