#ifndef DANMUCLIENT_H
#define DANMUCLIENT_H

#include <QObject>
#include <QTimer>


class QWebSocket;
class DanMuClient : public QObject
{
    Q_OBJECT
public:
    explicit DanMuClient(QObject *parent = nullptr);
    bool getBulletState();

public slots:
    void slot_loginServer(QString rid);
    void slot_logoutSever();

signals:
    void sig_lunchBullet(QString msg, QString name, QString board);


private:
    QByteArray buildMsg(QString b);
    QByteArray intTo4ByteArray(int i);
private slots:
    void slot_connected();
    void slot_byteReceived(QByteArray byte);
    void slot_sendHeartBeat();

private:
    QTimer *m_heartBeatTimer;
    QWebSocket *m_pWebSocket;
    QString m_rid;



};

#endif // DANMUCLIENT_H
