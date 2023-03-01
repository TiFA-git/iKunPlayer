#ifndef GETREALURL_H
#define GETREALURL_H

#include <QObject>
#include <QJsonObject>
#include <gethuya.h>
#include <getdouyu.h>

class GetRealUrl : public QObject
{
    Q_OBJECT
public:
    explicit GetRealUrl(QObject *parent = nullptr);


public:

public slots:
    void getRealUrl();
    void slots_convertUrl(QString, QJsonObject);

signals:
    void sig_getSuccess(QString, QJsonObject);
    void sig_getFinish();

private slots:

private:
    void loadPlayLst();

private:
    QList<QStringList> playLst;



};

#endif // GETREALURL_H
