#ifndef GETDOUYU_H
#define GETDOUYU_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QJsonArray>
#include <QRunnable>
#include <QJsonObject>

class GetDouYu : public QObject, public QRunnable
{
    Q_OBJECT
public:
    explicit GetDouYu(QObject *parent = nullptr);
    QJsonObject getRealUrl();
    void setRid(QString rid_);
    QString getName();
    void setNick(QString str);

signals:
    void sig_sendRes(QString, QJsonObject, QString);


public slots:

public:
    QString did;
    QString t10;
    QString t13;
    QString rid;
    QString s;
    QString res;
    QJsonArray rateObjLst;

protected:
    void run() override;

private:
    QString get_pre();
    QString get_md5(QString str);
    QString get_js();

private:
    QString nick;

};

#endif // GETDOUYU_H
