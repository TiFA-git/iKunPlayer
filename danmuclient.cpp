#include "danmuclient.h"
#include "comnetwork.h"

#include <QWebSocket>
#include <QUrl>
#include <QByteArray>
#include <QDebug>
#include <QTimer>
#include <QDateTime>
#include <QTextCodec>
#include <QRegExp>


DanMuClient::DanMuClient(QObject *parent)
    : QObject{parent},
      m_heartBeatTimer(nullptr)
{
    m_pWebSocket = new QWebSocket;
    connect(m_pWebSocket, &QWebSocket::connected, this, &DanMuClient::slot_connected);
    connect(m_pWebSocket, &QWebSocket::binaryMessageReceived, this, &DanMuClient::slot_byteReceived);
    m_heartBeatTimer = new QTimer();
    connect(m_heartBeatTimer, &QTimer::timeout, this, &DanMuClient::slot_sendHeartBeat);
}

bool DanMuClient::getBulletState()
{
    return m_pWebSocket->isValid();
}

void DanMuClient::slot_loginServer(QString rid)
{
    slot_logoutSever();  // 断开连接
    QUrl url = QUrl(QString("wss://danmuproxy.douyu.com:8506/"));
    m_rid = rid;           // 设置房间号， 在槽中加入弹幕组
    m_pWebSocket->open(url);  // connet信号发出后在曹函数加入弹幕组
}

void DanMuClient::slot_logoutSever()
{
    if(m_pWebSocket->isValid()){
        QString logoutReq = QString("type@=logout/");
        QByteArray logoutMsg = buildMsg(logoutReq);
        m_pWebSocket->sendBinaryMessage(logoutMsg);
        m_pWebSocket->close();
        m_heartBeatTimer->stop();
    }
}


QByteArray DanMuClient::buildMsg(QString loginReq)
{
    int dataLen = loginReq.size() + 9;
    QByteArray msgByte = loginReq.toUtf8();
    QByteArray lenByte = intTo4ByteArray(dataLen);
    QByteArray sendByte;
    sendByte.resize(4);
    sendByte[0] = 0xb1;
    sendByte[1] = 0x02;
    sendByte[2] = 0x00;
    sendByte[3] = 0x00;
    QByteArray data = lenByte + lenByte + sendByte + msgByte + sendByte.at(3);
    return data;
}

QByteArray DanMuClient::intTo4ByteArray(int i)
{
    QByteArray ba;
    ba.resize(4);
    ba[0] = (uchar)(0x000000ff & i);
    ba[1] = (uchar)((0x0000ff00&i)>>8);
    ba[2] = (uchar)((0x00ff0000&i)>>16);
    ba[3] = (uchar)((0xff000000&i)>>24);
    return ba;
}

void DanMuClient::slot_connected()
{

    qDebug() << "connected" << m_rid;
    m_heartBeatTimer->start(40000);
    QString loginReq = QString("type@=loginreq/roomid@=%1/").arg(m_rid);
    QString groupReq = QString("type@=joingroup/rid@=%1/gid@=-9999/").arg(m_rid);
    QByteArray loginMsg = buildMsg(loginReq);
    m_pWebSocket->sendBinaryMessage(loginMsg);
    QByteArray groupMsg = buildMsg(groupReq);
    m_pWebSocket->sendBinaryMessage(groupMsg);
}

void DanMuClient::slot_byteReceived(QByteArray byte)
{
    QTextCodec *tc = QTextCodec::codecForName("utf-8");
    QString str = tc->toUnicode(byte);
    QStringList tmp = str.split("type@=chatmsg");
    foreach(QString s, tmp){
        QString txt = ComNetWork::search("txt@=(.*)/", s, 1, true);
        if(txt.isEmpty()){
            continue;
        }
        QString nn = ComNetWork::search("nn@=(.*)/", s, 1, true);
        QString bnn = ComNetWork::search("bnn@=(.*)/", s, 1, true);
        emit sig_lunchBullet(txt, nn, bnn);
    }
}

void DanMuClient::slot_sendHeartBeat()
{
    QDateTime dataTime = QDateTime::currentDateTime();
    qint64 time = dataTime.toSecsSinceEpoch();
    QString heartBeatReq = QString("type@=keeplive/tick@=%1/").arg(time);
    QByteArray heartBeatMsg = buildMsg(heartBeatReq);
    m_pWebSocket->sendBinaryMessage(heartBeatMsg);
}
