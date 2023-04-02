#ifndef MPVPLAYERWIDGET_H
#define MPVPLAYERWIDGET_H

#include <QWidget>
#include <QObject>
#include <QDebug>
#include <client.h>  // mpv头文件
#include <QTimer>
#include <QKeyEvent>

namespace Ui {
class MpvPlayerWidget;
}

class MpvPlayerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MpvPlayerWidget(QWidget *parent = nullptr);
    ~MpvPlayerWidget();
    QString getProperty(const QString &name) const; // 获得mpv属性
    void play(QString filename); // 播放视频

public slots:
    void slot_setProperty(QString name, QString value); // 设置mpv属性
    void slot_CMD(QString, QString);



signals:
    void mpv_events(); // 触发on_mpv_events()槽函数的信号
    void mpv_palyEnd(); // 播放结束的信号
    void send_setFullScreen();
    void sig_pause();


protected:
    void mouseDoubleClickEvent(QMouseEvent *event);
    void mousePressEvent (QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);

private:
    void creatMpvPlayer();
    void handle_mpv_event(mpv_event *event); // 处理mpv事件


private slots:
    void on_mpv_events(); // 这个槽函数由 wakeup()调用（通过mpv_events信号）
    void onTimeCursorHide();
    void slot_mouseClickCnt();

private:
    Ui::MpvPlayerWidget *ui;
    mpv_handle *mpv;
    QTimer *hideCursor;
    QTimer *mouseClickTimer;
};

#endif // MPVPLAYERWIDGET_H
