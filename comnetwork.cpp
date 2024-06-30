#include "comnetwork.h"

ComNetWork::ComNetWork(QObject *parent) : QObject(parent)
{

}

QByteArray ComNetWork::get(const QString &strUrl, bool useHeader)
{
    assert(!strUrl.isEmpty());
    const QUrl url = QUrl::fromUserInput(strUrl);
    assert(url.isValid());

    QNetworkRequest qnr(url);
    if(useHeader){
        qnr.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
        qnr.setHeader(QNetworkRequest::UserAgentHeader,
                      "Mozilla/5.0 (Linux; Android 5.0; SM-G900P Build/LRX21T) "
                      "AppleWebKit/537.36 (KHTML, like Gecko) "
                      "Chrome/75.0.3770.100 Mobile Safari/537.36 ");
        qnr.setRawHeader("Accept", "*/*");
        qnr.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
    }
    QNetworkAccessManager m_qnam;
    QNetworkReply* reply = m_qnam.get(qnr); //m_qnam是QNetworkAccessManager对象

    QEventLoop eventLoop;
    connect(reply, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);
    eventLoop.exec(QEventLoop::ExcludeUserInputEvents);

    QByteArray replyData = reply->readAll();
    reply->deleteLater();
    reply = nullptr;

    return replyData;
}

QByteArray ComNetWork::post(const QString &strUrl, QJsonObject headers, QJsonObject data)
{
    assert(!strUrl.isEmpty());
    const QUrl url = QUrl::fromUserInput(strUrl);
    assert(url.isValid());

    QNetworkRequest qnr(url);
    qnr.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    if(!headers.isEmpty()){
        foreach (QString key, headers.keys()) {
            qnr.setRawHeader(key.toUtf8(), headers.value(key).toString().toUtf8());
        }
    }
    QString dataStr = QString();
    for(int i= 0; i < data.keys().size(); i ++){
        QString key = data.keys().at(i);
        QString value = data.value(key).toString();
        QString tmp;
        if(i != data.keys().size() - 1){
            tmp = QString("%1=%2&").arg(key, value);
        }else{
            tmp = QString("%1=%2").arg(key, value);
        }
        dataStr += tmp;
    }
    QNetworkAccessManager m_qnam;
    QNetworkReply* reply = m_qnam.post(qnr, dataStr.toUtf8());

    QEventLoop eventLoop;
    connect(reply, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);
    eventLoop.exec(QEventLoop::ExcludeUserInputEvents);

    QByteArray replyData = reply->readAll();
    reply->deleteLater();
    reply = nullptr;
    return replyData;
}

QByteArray ComNetWork::postJson(const QString &strUrl, QJsonObject headers, QJsonObject data)
{
    assert(!strUrl.isEmpty());
    const QUrl url = QUrl::fromUserInput(strUrl);
    assert(url.isValid());

    QNetworkRequest qnr(url);
    qnr.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    if(!headers.isEmpty()){
        foreach (QString key, headers.keys()) {
            qnr.setRawHeader(key.toUtf8(), headers.value(key).toString().toUtf8());
        }
    }

    QNetworkAccessManager m_qnam;
    QNetworkReply* reply = m_qnam.post(qnr, QJsonDocument(data).toJson());

    QEventLoop eventLoop;
    connect(reply, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);
    eventLoop.exec(QEventLoop::ExcludeUserInputEvents);

    QByteArray replyData = reply->readAll();
    reply->deleteLater();
    reply = nullptr;
    return replyData;
}

QString ComNetWork::search(QString re, QString str, int group, bool b)
{
    if(group < 0){
        return QString();
    }
    QRegExp rx_1(re);
    rx_1.setMinimal(b);
    QString str_1 = str;
    int pos = rx_1.indexIn(str_1);
    if(pos>=0)
    {
        return rx_1.cap(group);
    }
    return QString();
}
