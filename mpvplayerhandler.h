#ifndef MPVPLAYERHANDLER_H
#define MPVPLAYERHANDLER_H

#include <QObject>
#include <QDebug>
#include <client.h>  // mpv头文件
#include <QTimer>
#include <QKeyEvent>

namespace Ui {
class MpvPlayer;
}

class MpvPlayerHandler : public QObject
{
    Q_OBJECT

public:
    explicit MpvPlayerHandler(QObject *parent = nullptr);
    ~MpvPlayerHandler();
    QString getProperty(const QString &name) const; // 获得mpv属性
    void play(QString filename); // 播放视频
    void stop();
    void setWinID(int64_t);

public slots:
    void slot_setProperty(QString name, QString value); // 设置mpv属性


signals:
    void mpv_events(); // 触发on_mpv_events()槽函数的信号
    void mpv_palyEnd(); // 播放结束的信号


protected:


private:
    void creatMpvPlayer();
    void handle_mpv_event(mpv_event *event); // 处理mpv事件


private slots:
    void on_mpv_events(); // 这个槽函数由 wakeup()调用（通过mpv_events信号）


private:
    mpv_handle *mpv;
    QTimer *hideCursor;
    QTimer *mouseClickTimer;
};

#endif // MPVPLAYERHANDLER_H
