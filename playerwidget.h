#ifndef PLAYERWIDGET_H
#define PLAYERWIDGET_H

#include <QWidget>
#include <QShowEvent>
#include <QHideEvent>
#include <QResizeEvent>
#include <QMoveEvent>
#include <QTimer>


class PlayerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PlayerWidget(QWidget *parent = nullptr);


    QWidget *topLayerWidget() const;
    void setTopLayerPos(QPoint pos);


signals:
    void send_setFullScreen();
    void sig_pause();
    void sig_showCtrl(bool);


protected:
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void mousePressEvent (QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private slots:
    void onTimeCursorHide();
    void slot_mouseClickCnt();


private:
    void initTopLayer(); // 初始化顶层widget
    void initMouseTimer();  // 初始化鼠标相关定时器



private:
    QWidget *m_topLayerWidget;
    QTimer *hideCursor;
    QTimer *mouseClickTimer;

};

#endif // PLAYERWIDGET_H
