#ifndef GETHUYA_H
#define GETHUYA_H
#include <QJsonObject>
#include <QObject>
#include <QRunnable>

class GetHuYa : public QObject, public QRunnable
{
    Q_OBJECT
public:
    explicit GetHuYa(QObject *parent = nullptr);

    void get_real_url();
    void setRid(QString);
    void setNick(QString);

protected:
    void run();

signals:
    void sig_sendUrl(QString, QJsonObject, QString);

private:
    QString live(QString e);
    QString get_md5(QString str);
    QString getTime17();


private:
    QString m_rid;
    QString Nick;

};

#endif // GETHUYA_H
