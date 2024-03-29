#include "mpvplayerhandler.h"
#include <QDebug>


// 唤醒函数
static void wakeup(void *ctx)
{
    // 此回调可从任何mpv线程调用（但也可以从调用mpv API的线程递归地返回）
    // 只是需要通知要唤醒的Qt GUI线程（以便它可以使用mpv_wait_event（）），并尽快返回
    MpvPlayerHandler *mvpPlayer = (MpvPlayerHandler *)ctx;
    emit mvpPlayer->mpv_events();
}

MpvPlayerHandler::MpvPlayerHandler(QObject *parent) :
    QObject(parent)
{
    creatMpvPlayer();
}


MpvPlayerHandler::~MpvPlayerHandler()
{
    delete mpv;
    delete hideCursor;
    delete mouseClickTimer;
}

void MpvPlayerHandler::creatMpvPlayer()
{
    // 创建mpv实例
    mpv = mpv_create();
    if (!mpv)
        throw std::runtime_error("can't create mpv instance");

    // 让我们通过MPV_EVENT_PROPERTY_CHANGE接收属性更改事件，如果这属性更改了
    mpv_observe_property(mpv, 0, "time-pos", MPV_FORMAT_DOUBLE);

    // 从这里开始，将调用唤醒功能。 回调可以来自任何线程，因此我们使用QueuedConnection机制以线程安全的方式中继唤醒
    connect(this, &MpvPlayerHandler::mpv_events, this, &MpvPlayerHandler::on_mpv_events,
            Qt::QueuedConnection);
    mpv_set_wakeup_callback(mpv, wakeup, this);

    // 判断mpv实例是否成功初始化
    if (mpv_initialize(mpv) < 0)
        throw std::runtime_error("mpv failed to initialize");
}

void MpvPlayerHandler::slot_setProperty(QString name, QString value)
{
    qDebug() << __FUNCTION__ << name << value;
    mpv_set_option_string(mpv, name.toLatin1().data(), value.toLatin1().data());
}


QString MpvPlayerHandler::getProperty(const QString &name) const
{
    return (QString)mpv_get_property_string(mpv, name.toLatin1().data());
}

void MpvPlayerHandler::play(QString filename)
{
    if (mpv)
    {
        const QByteArray c_filename = filename.toUtf8();
        const char *args[] = {"loadfile", c_filename.data(), NULL};
        // 与mpv_command相同，但异步运行命令来避免阻塞，直到进程终止
        mpv_command_async(mpv, 0, args);
        qDebug() << __FUNCTION__ << c_filename;
    }
}

void MpvPlayerHandler::stop()
{
    if(mpv){
        const char *args[] = {"stop", NULL, NULL};
        // 与mpv_command相同，但异步运行命令来避免阻塞，直到进程终止
        mpv_command_async(mpv, 0, args);
    }
}

void MpvPlayerHandler::setWinID(int64_t wid)
{
    // 将视频子窗口的窗口ID传递给mpv wid选项
    mpv_set_option(mpv, "wid", MPV_FORMAT_INT64, &wid);
}


void MpvPlayerHandler::on_mpv_events()
{
    // 处理所有事件，直到事件队列为空
    while (mpv)
    {
        mpv_event *event = mpv_wait_event(mpv, 0);
        if (event->event_id == MPV_EVENT_NONE)
            break;
        handle_mpv_event(event);
    }
}


void MpvPlayerHandler::handle_mpv_event(mpv_event *event)
{
    switch (event->event_id)
    {
        // 属性改变事件发生
        case MPV_EVENT_PROPERTY_CHANGE:
        {
            mpv_event_property *prop = (mpv_event_property *)event->data;

            if (strcmp(prop->name, "time-pos") == 0)
            {
                if (prop->format == MPV_FORMAT_DOUBLE)
                {
                    // 获得播放时间
                    //double time = *(double *)prop->data;
                    //qDebug() << "播放时间: " << QString::number(time, 10, 2);
                }
                else if (prop->format == MPV_FORMAT_NONE)
                {
                    // 该属性不可用，可能意味着播放已停止
                    emit mpv_palyEnd();
                    qDebug() << "播放结束";
                }
            }
        }
        break;

        // palyer退出事件发生
        case MPV_EVENT_SHUTDOWN:
        {
            mpv_terminate_destroy(mpv);
            mpv = NULL;
        }
        break;

        default: ;
    }
}
