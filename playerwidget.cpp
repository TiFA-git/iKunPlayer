#include "playerwidget.h"

#include <QDebug>
#include <QLabel>
#include <QVBoxLayout>

PlayerWidget::PlayerWidget(QWidget *parent)
    : QWidget{parent}
{
    setMouseTracking(true); // 确保mouseMove的时候即触发事件
    initTopLayer();
    initMouseTimer();
}

void PlayerWidget::showEvent(QShowEvent *event)
{
    m_topLayerWidget->show();
    event->ignore();
}

void PlayerWidget::hideEvent(QHideEvent *event)
{
    m_topLayerWidget->hide();
    event->ignore();
}

void PlayerWidget::resizeEvent(QResizeEvent *event)
{
    m_topLayerWidget->setGeometry(this->geometry());
    event->ignore();
}

void PlayerWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    mouseClickTimer->stop();  // 双击命令， 不要响应单击
    emit send_setFullScreen();
    event->accept();
}

void PlayerWidget::mousePressEvent(QMouseEvent *event)
{
    if(event->button() != Qt::LeftButton){
        event->accept();
        return;
    }
    //    mouseClickTimer->start();
}

void PlayerWidget::mouseMoveEvent(QMouseEvent *event)
{
    setCursor(Qt::ArrowCursor);
    if(hideCursor->isActive())
        hideCursor->stop();
    hideCursor->start();
    emit sig_showCtrl(true);
    QWidget::mouseMoveEvent(event);
}

void PlayerWidget::onTimeCursorHide()
{
    setCursor(Qt::BlankCursor);
    hideCursor->stop();
    emit sig_showCtrl(false);
}

void PlayerWidget::slot_mouseClickCnt()
{
    emit sig_pause();
    mouseClickTimer->stop();
}

void PlayerWidget::initTopLayer()
{
    m_topLayerWidget = new QWidget(this);
    m_topLayerWidget->setAttribute(Qt::WA_TranslucentBackground);
    m_topLayerWidget->setAttribute(Qt::WA_ShowWithoutActivating);
    m_topLayerWidget->setAttribute(Qt::WA_TransparentForMouseEvents);
    m_topLayerWidget->setWindowFlags(Qt::Tool|Qt::FramelessWindowHint);
}

void PlayerWidget::initMouseTimer()
{
    hideCursor = new QTimer(this);  // 计时隐藏指针
    hideCursor->setInterval(2000);
    connect(hideCursor, &QTimer::timeout, this, &PlayerWidget::onTimeCursorHide);


    mouseClickTimer = new QTimer;  // 计算200ms 内鼠标点击次数, 手动实现鼠标单击
    mouseClickTimer->setInterval(200);
    connect(mouseClickTimer, &QTimer::timeout, this, &PlayerWidget::slot_mouseClickCnt);
}


QWidget *PlayerWidget::topLayerWidget() const
{
    return m_topLayerWidget;
}

void PlayerWidget::setTopLayerPos(QPoint pos)
{
    m_topLayerWidget->move(pos);
}
