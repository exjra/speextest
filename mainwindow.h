#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "haudiomanager.h"

#if defined(HAS_HARFSDK)
#if !defined (__ANDROID__)
#include "haudioclient.h"
#endif
#endif

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void setAec(HAudioManager *aec);

#if defined(HAS_HARFSDK)
#if !defined (__ANDROID__)
    void setClient(HAudioClient *client);
#endif
#endif

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_6_clicked();

    void onSpeechState(bool pSpeech);
    void onTimeDiff(int pdiff);

    void on_pushButton_7_clicked();

    void on_pushButton_8_clicked();

private:
    Ui::MainWindow *ui;
    HAudioManager* mAudioManager;

#if defined(HAS_HARFSDK)
    #if !defined (__ANDROID__)
    HAudioClient* mClient;
#endif
#endif
};

#endif // MAINWINDOW_H
