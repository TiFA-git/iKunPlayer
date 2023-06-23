#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QListWidgetItem>
#include <QObject>
#include <QJsonObject>
#include <QMouseEvent>
#include <QStackedLayout>
#include <QMoveEvent>

#include "getdouyu.h"
#include "gethuya.h"
#include "mpvplayerhandler.h"
#include "getrealurl.h"


class Controller;
class PlayerListWidget;
class DanMuClient;
class Bullet;
class BulletPad;
class PlayerWidget;


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
    void sig_loginBulletClient(QString rid);
    void sig_stopDanMuClient();
    void sig_clearBullet();

protected:
    void moveEvent(QMoveEvent *);

private slots:
    void on_playPushButton_clicked(); // 播放/停止
    void on_pausePushButton_clicked(); // 暂停/恢复视频
    void paltEnd_slot(); // 接受播放结束信号的槽函数
    void on_recordPushButton_clicked();
    void on_pushButton_clicked();

    void on_listWidget_itemClicked(QListWidgetItem *item);
    void on_rateSelect_currentTextChanged(const QString &arg1);
    void onProcessFullScreen();
    void slot_recRes(QString, QJsonObject, QString);
    void slot_updateUrls();

    void on_pushButton_2_clicked();
    void slot_pause();
    void slot_updateUI();
    void slot_processCtrlShow(bool b);
    void slot_taggleLst();
    void slot_receivedBullet(QString msg, QString name, QString board);
    void slot_accurated(QObject *);

    void on_checkBox_toggled(bool checked);
    void slot_maxBulletNum(int value);
    void slot_BulletRowCnt(int value);
    void slot_BulletSize(int value);
    void slot_toggleBullet(bool checked);
    void slot_showBulletPad();

private:
    void initPlayer();      //  初始化播放器
    void initControllerWidget();    //  初始化全屏时播放控制器
    void initRealUrl();     //  初始化推流地址

    bool isPlay(); // 判断是否在播放
    bool isRecord(); // 判断是否在录制
    void loadPlayLst();
    void threadDYRid(QString nick, QString rid);
    void initListWidget();
    void initBulletClient();
    void toggleDanMu(bool b);
    void initBulletPad();



private:
    Ui::MainWindow *ui;
    bool m_isPlay;
    bool m_isRecord;
    QList<QStringList> playLst;
    QMap<QString, QJsonObject> realUrlsMap;  // 收到的直播源
    QMap<QString, QString> m_nick2Rid;    //  昵称2房间号， 用于弹幕接受
    QMap<QString, QJsonObject> realUrlsMapUI;  // 界面显示直播源，较旧
    QJsonObject m_curUrlObj;
    bool m_isFullScreen;
    QTimer *hideCursor;
    QTimer *onTimeUpdateUrls;
    GetRealUrl *urlGetter;
    QThread *loadThread;
    Controller *m_controllerWidget;
    PlayerListWidget *playerListWidget;
    DanMuClient *m_bulletClient;
    QThread *m_bulletThread;
    bool isAllowDanMu;
    QString m_curRid;
    BulletPad *m_bulletPad;
    int m_curBulletCnt;  // 当前弹幕数
    int m_maxBulletNum;  // 屏幕上最大弹幕数
    int m_rowNum;
    int m_fontSize;
    MpvPlayerHandler *m_player;
    PlayerWidget *m_playerWidget;
    QWidget *m_topLayerWidget;
    QString m_curUrl;  // 当前播放的url
};

#endif // MAINWINDOW_H
