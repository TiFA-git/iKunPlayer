#include "bullet.h"
#include <QRandomGenerator>
#include <QDebug>
#include <QDesktopWidget>

Bullet::Bullet(QObject *parent)
    : QObject{parent},
      m_topLayer(nullptr),
      m_fontSize(20)
{
    QWidget *tmp = static_cast<QWidget *>(parent);
    if(tmp){
        m_topLayer = tmp;
    }else{
        return;
    }

    m_label = new QLabel(m_topLayer);
    m_label->setFocusPolicy(Qt::NoFocus);
    m_label->setMinimumWidth(500);
    m_label->setAttribute(Qt::WA_TranslucentBackground);
    m_label->setAttribute(Qt::WA_ShowWithoutActivating);
    m_label->setAttribute(Qt::WA_TransparentForMouseEvents);
//    m_label->setWindowFlags(Qt::FramelessWindowHint | Qt::SplashScreen | Qt::WindowStaysOnTopHint);
    m_label->setWindowFlags(Qt::FramelessWindowHint  | Qt::WindowStaysOnTopHint);
    m_label->setWindowFlags(Qt::WindowStaysOnTopHint);
//    m_label->setWindowFlags(Qt::Tool);
    m_label->setStyleSheet(QString("color:%1;font: 75 20px \"微软雅黑\"").arg(QColor::colorNames().at(QRandomGenerator::global()->bounded(QColor::colorNames().count()))));
    m_animation = new QPropertyAnimation(m_label, "pos", this);
    connect(m_animation, SIGNAL(finished()), this, SLOT(slot_amFinished()));
}

void Bullet::shoot(QString msg, int lineNum)
{
    if(m_topLayer == nullptr){
        return;
    }
    m_label->move(m_topLayer->width() + QRandomGenerator::global()->bounded(1000) , lineNum * m_fontSize);
    m_label->setText(msg);
    m_label->show();
    int wordCnt = msg.size();
    int duration;
    if(wordCnt < 15){
        duration = 10000;
    }else if(wordCnt < 30){
        duration = 8000;
    }else{
        duration = 6000;
    }
    m_animation->setStartValue(QPoint(m_label->x(), m_label->y()));
    m_animation->setEndValue(QPoint(-m_label->width(),  m_label->y()));
    m_animation->setDuration(QRandomGenerator::global()->bounded(duration, duration + 2000));
    m_animation->start();
}

Bullet::~Bullet()
{
    qDebug() << "delete";
    m_label->deleteLater();
    m_label = nullptr;
}

int Bullet::setFontSize(int size)
{
    m_fontSize = size;
    m_label->setStyleSheet(QString("color:%1;font: 75 %2px \"微软雅黑\"")
                           .arg(QColor::colorNames().at(QRandomGenerator::global()->bounded(QColor::colorNames().count())))
                           .arg(size));
    return m_topLayer->height() / size;
}

int Bullet::getLineCnt()
{
    return m_topLayer->height() / m_fontSize;
}

void Bullet::slot_amFinished()
{
    emit sig_accurated(this);
}

void Bullet::slot_destroyBullet()
{
    m_animation->stop();
    m_label->hide();
    emit m_animation->finished();  // 直接去外部析构
}
