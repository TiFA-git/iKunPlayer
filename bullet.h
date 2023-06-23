#ifndef BULLET_H
#define BULLET_H

#include <QObject>
#include <QFont>
#include <QLabel>
#include <QPropertyAnimation>

class Bullet : public QObject
{
    Q_OBJECT
public:
    explicit Bullet(QObject *parent = nullptr);
    void shoot(QString msg, int px);
    ~Bullet();
    int setFontSize(int);
    int getLineCnt();

signals:
    void sig_accurated(QObject *);

private slots:
    void slot_amFinished();

public slots:
    void slot_destroyBullet();


private:
    int m_fontSize;
    QLabel *m_label;
    QPropertyAnimation *m_animation;
    QWidget *m_topLayer;
};

#endif // BULLET_H
