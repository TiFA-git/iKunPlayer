#include "gethuya.h"
#include "comnetwork.h"
#include <QRegExp>
#include <QDateTime>

#include <QSslSocket>
#include <QDebug>


#include <iostream>
#include <sys/time.h>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <cmath>
#include <unistd.h>

using namespace std;


GetHuYa::GetHuYa(QObject *parent) : QObject(parent)
{

}

void GetHuYa::get_real_url()
{
    QJsonObject real_lists;
    QString real_url;
    QString room_url = QString("https://m.huya.com/%1").arg(m_rid);
//    QString room_url = "http://00f06771-a453-4d77-b65d-f9ffa115c86d.mock.pstmn.io";
    QByteArray response_ = ComNetWork::get(room_url, true);
    QString response  = QString::fromUtf8(response_);
    QString name = ComNetWork::search(QString("\"sNick\"\\:\"(.*)\"\\,\"iSex\""), response, 1, true);
    if(!name.isEmpty()){
        Nick = name;
    }
    QString liveLineUrl = ComNetWork::search(QString("\"liveLineUrl\"\\:\"([\\s\\S]*)\"\\,"), response, 1, true);
    QByteArray liveline_ = QByteArray::fromBase64(liveLineUrl.toUtf8());
    QString liveline = QString::fromUtf8(liveline_);
    if(liveline.contains("replay")){
        real_lists.insert("直播录像", "http:" + liveline);
    }else{
        liveline = live(liveline);
        if(liveline.isEmpty()){
            return;
        }
        real_url = QString("http:") + liveline.replace("hls", "flv").replace("m3u8", "flv").remove("&ctype=tars_mobile");
    }
    QStringList rate = {"10000", "8000", "4000", "2000", "500"};
    if(!real_url.isEmpty()){
        foreach (QString ratio, rate) {
            if(ratio != QString("10000")){
                QString real_url_flv = real_url.replace(QString(".flv?"), QString(".flv?radio=%1&").arg(ratio));
                real_lists.insert(QString("flv_%1").arg(ratio), real_url_flv);
            }else{
                real_lists.insert(QString("flv_%1").arg(ratio), real_url);
            }
        }
    }
    emit sig_sendUrl(Nick, real_lists, m_rid);
    return;
}

void GetHuYa::run()
{
    get_real_url();
}

QString GetHuYa::live(QString e)
{
    QStringList i_b = e.split("?");
    if(i_b.first().isEmpty()){
        return QString();
    }
    QStringList r = i_b.first().split("/");
    QString s = r.last().remove(QString(".flv")).remove(".m3u8");
    QStringList c_ = i_b.last().split("&");
    QStringList c;
    QString c_last = QString();
    for(int i = 0; i < c_.size(); i++){
        QString str = c_.at(i);
        if(i < 3){
            c << str;
        }else{
            c_last += QString("&") + str;
        }
    }
    if(!c_last.isEmpty())
        c << c_last.mid(1);
    c.removeAll(QString(" "));

    QMap<QString, QString> n;
    foreach (QString c_item, c) {
        QStringList c_item_lst = c_item.split("=");
        if(c_item.size() < 2){
            continue;
        }
        n.insert(c_item_lst.at(0), c_item_lst.at(1));
    }
    QString uid = get_anonymous_uid();
    QString seqid = QString::number( getTime17().toLongLong() + uid.toLongLong());
    QString t = n.value("t");
    QString ctype = n.value("ctype");
    QString ss = QString("%1|%2|%3").arg(seqid).arg(ctype).arg(t);
    ss = get_md5(ss);
    QString wsTime= n.value("wsTime");
    QString fm = n.value("fm");
    fm.replace("%3F", "?");
    fm.replace("%2F", "/");
    fm.replace("%25", "%");
    fm.replace("%23", "#");
    fm.replace("%26", "&");
    fm.replace("%3D", "=");
    QByteArray u_ = QByteArray::fromBase64(fm.toUtf8());
    QString u = QString::fromUtf8(u_);
    fm = u.split('_').at(0);
    QString h = fm + QString("_") + uid + QString("_")
                                + s + QString("_") + ss + QString("_") + wsTime;
    QString m = get_md5(h);
    QString y = c.last();
    QString uuid = get_uuid();
    QString url = i_b.first() + QString("?wsSecret=") + m +
            QString("&wsTime=") + wsTime +
            QString("&seqid=") + seqid  + QString("&uid=") + uid +
            QString("&uuid=") + uuid + QString("&ver=1&sv=2110211124") ;
    return url;
}

QString GetHuYa::get_md5(QString str)
{
    QString md5Str = QCryptographicHash::hash(str.toUtf8(),QCryptographicHash::Md5).toHex();
    return md5Str;
}

QString GetHuYa::getTime17()
{
    struct timespec tn;
    clock_gettime(CLOCK_REALTIME, &tn);
    qint64 tv_sec = tn.tv_sec;
    qint64 tv_nsec = tn.tv_nsec / 100;
    return QString::number(tv_sec) + QString::number(tv_nsec);
}

QString GetHuYa::get_anonymous_uid()
{
    QString url = "https://udblgn.huya.com/web/anonymousLogin";
    QJsonObject headers;
    headers.insert("appId", 5002);
    headers.insert("byPass", 3);
    headers.insert("context", "");
    headers.insert("version", "2.4");
    headers.insert("data", QJsonObject());
    QByteArray byte = ComNetWork::postJson(url, QJsonObject(), headers);
    QJsonDocument doc = QJsonDocument::fromJson(byte);
    QJsonObject obj = doc.object();
    QJsonObject data = obj.value("data").toObject();
    QString uid = data.value("uid").toString();
    return uid;
}

QString GetHuYa::get_uuid()
{
    qint64 now = getTime17().toLongLong();
    qint64 res = now % 10000000000 * 1000 + (qrand() % 1000);
    return QString::number(res % 4294967295);
}

void GetHuYa::setRid(QString rid_)
{
    m_rid = rid_;
}

void GetHuYa::setNick(QString nick_)
{
    Nick = nick_;
}
