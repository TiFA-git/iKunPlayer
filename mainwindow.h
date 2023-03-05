#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QListWidgetItem>
#include <QObject>
#include <QJsonObject>
#include <QMouseEvent>
#include "getdouyu.h"
#include "gethuya.h"
#include "mpvplayerwidget.h"
#include "getrealurl.h"


class Controller;
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void sig_runGet();

private slots:
    void on_playPushButton_clicked(); // 播放/停止
    void on_pausePushButton_clicked(); // 暂停/恢复视频
    void paltEnd_slot(); // 接受播放结束信号的槽函数
    void on_recordPushButton_clicked();
    void on_pushButton_clicked();

    void on_listWidget_itemClicked(QListWidgetItem *item);
    void on_rateSelect_currentTextChanged(const QString &arg1);
    void onProcessFullScreen();
    void slot_recRes(QString, QJsonObject);
    void slot_updateUrls();

    void on_pushButton_2_clicked();
    void slot_pause();
    void slot_updateUI();

private:
    Ui::MainWindow *ui;
    void initControllerWidget();
    bool isPlay(); // 判断是否在播放
    bool isRecord(); // 判断是否在录制
    void loadPlayLst();
    void threadDYRid(QString nick, QString rid);


private:
    MpvPlayerWidget *mpvPlayer;
    bool m_isPlay;
    bool m_isRecord;
    QList<QStringList> playLst;
    QMap<QString, QJsonObject> realUrlsMap;  // 收到的直播源
    QMap<QString, QJsonObject> realUrlsMapUI;  // 界面显示直播源，较旧
    QJsonObject curUrlObj;
    bool isFullScreen;
    QTimer *hideCursor;
    QTimer *onTimeUpdateUrls;
    GetRealUrl *urlGetter;
    QThread *loadThread;
    Controller *controllerWidget;

};

#endif // MAINWINDOW_H
