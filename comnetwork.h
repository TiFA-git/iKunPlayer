#ifndef COMNETWORK_H
#define COMNETWORK_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QEventLoop>
#include <QRegExp>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDebug>

class ComNetWork : public QObject
{
    Q_OBJECT
public:
    explicit ComNetWork(QObject *parent = nullptr);

signals:


public:
    static QByteArray get(const QString &strUrl, bool useHeader = false);
    static QByteArray post(const QString &strUrl, QJsonObject headers = QJsonObject(), QJsonObject data = QJsonObject());
    static QByteArray postJson(const QString &strUrl, QJsonObject headers = QJsonObject(), QJsonObject data = QJsonObject());
    static QString search(QString re, QString str, int group, bool b = false);

};

#endif // COMNETWORK_H
