#include "getdouyu.h"
#include "comnetwork.h"
#include <QDateTime>
#include <QDebug>
#include <QUrl>
#include <QNetworkReply>
#include <QTextCodec>
#include <QEventLoop>
#include <QCryptographicHash>
#include <QJsonDocument>
#include <QJSEngine>
#include <QJSValueList>
#include <QRunnable>


GetDouYu::GetDouYu(QObject *parent) : QObject(parent)
{
    did = QString("10000000000000000000000000001501");
}

QJsonObject GetDouYu::getRealUrl()
{
    QString result = ComNetWork::search("rid\":(\\d{1,8}),\"vipId", res, 1);
    if(result.isEmpty()){
        return QJsonObject();
    }
    rid = result;
    QString key;
    key = get_pre();
    if(key.isEmpty()){
        key = get_js();
    }
    if(key.isEmpty()){
        return QJsonObject();
    }
    QString name = ComNetWork::search("\"nickname\":\"(.*)\"", res, 1, true);
    if(!name.isEmpty()){
        nick = name;
    }
    if(rateObjLst.isEmpty()){
        QJsonObject tmpBlueray;
        tmpBlueray.insert("name", "蓝光");
        tmpBlueray.insert("rate", 0);
        tmpBlueray.insert("high_bit", 1);
        QJsonObject tmpSuperClear;
        tmpSuperClear.insert("name", "超清");
        tmpSuperClear.insert("rate", 3);
        tmpSuperClear.insert("high_bit", 0);
        QJsonObject tmpHeightClear;
        tmpHeightClear.insert("name", "高清");
        tmpHeightClear.insert("rate", 2);
        tmpHeightClear.insert("high_bit", 0);
        rateObjLst << tmpBlueray << tmpSuperClear << tmpHeightClear;
    }
    QJsonObject resObj;
    foreach (QJsonValue curValue, rateObjLst) {
        QJsonObject curRate = curValue.toObject();
        if(!(curRate.contains("name")))
            continue;
        if(curRate.contains("rate") && curRate.value("rate").toInt() > 0){
            resObj.insert(curRate.value("name").toString() + QString("dyp2p-huos"),
                       QString("https://tc-tc2-interact.douyucdn2.cn/live/%1_%2.flv?uuid=").arg(key, QString::number(curRate.value("rate").toInt() * 1000)));
//            resObj.insert(curRate.value("name").toString() + QString(" hdltctwk"),
//                        QString("http://hdltctwk.douyucdn2.cn/live/%1_%2.flv?uuid=").arg(key, QString::number(curRate.value("rate").toInt() * 1000)));
//            resObj.insert(curRate.value("name").toString() + QString("aliyun"),
//                        QString("http://dyscdnali1.douyucdn.cn/live/%1_%2.flv?uuid=").arg(key, QString::number(curRate.value("rate").toInt() * 1000)));
        }else{
            resObj.insert(curRate.value("name").toString() + QString("dyp2p-huos"),
                       QString("https://tc-tc2-interact.douyucdn2.cn/live/%1.flv?uuid=").arg(key));
//            resObj.insert(curRate.value("name").toString() + QString(" hdltctwk"),
//                        QString("http://hdltctwk.douyucdn2.cn/live/%1.m3u8?txSecret=b371cdb3727a41fbeb4e52cd975b4f64&txTime=64870df7").arg(key));
//            resObj.insert(curRate.value("name").toString() + QString("aliyun"),
//                        QString("http://dyscdnali1.douyucdn.cn/live/%1.flv?uuid=").arg(key));
        }
    }
    emit sig_sendRes(nick, resObj, rid);
    return resObj;
}

void GetDouYu::setRid(QString rid_)
{
    rid = rid_;
    QDateTime dataTime = QDateTime::currentDateTime();
    qint64 secTime = dataTime.toSecsSinceEpoch();
    qint64 msectime = dataTime.toMSecsSinceEpoch();
    t10 = QString::number(secTime);
    t13 = QString::number(msectime);
    res =  QString::fromUtf8(ComNetWork::get(QString("http://m.douyu.com/") + rid));
}

QString GetDouYu::getName()
{
    QString name = ComNetWork::search("\"nickname\":\"(.*)\"", res, 1, true);
    if(!name.isEmpty()){
        nick = name;
    }
    return name;
}

void GetDouYu::setNick(QString str)
{
    nick = str;
}

QString GetDouYu::get_pre()
{
    QJsonObject headers;
    QJsonObject data;
    headers.insert("rid", rid);
    headers.insert("time", t13);
    QString auth = get_md5(rid + t13);
    headers.insert("auth", auth);
    data.insert("rid", rid);
    data.insert("did", did);
    QString url = QString("http://playweb.douyucdn.cn/lapi/live/hlsH5Preview/") + rid;
    QByteArray res = ComNetWork::post(url, headers, data);
    if(res.isEmpty())
        return QString();
    QJsonObject resJson = QJsonDocument::fromJson(res).object();
    if(!resJson.contains("data"))
        return QString();
    QJsonObject resData = resJson.value("data").toObject();
    if(resData.contains("settings")){
        QJsonArray rateJsonArray = resData.value("settings").toArray();
        foreach (auto tmpObj, rateJsonArray) {
            rateObjLst.append(tmpObj);
        }
    }
    if(!resData.contains("rtmp_live"))
        return QString();
    QString rtmp_live = resData.value("rtmp_live").toString();
    if(rtmp_live.isEmpty())
        return QString();
    QString key = ComNetWork::search("(\\d{1,8}[0-9a-zA-Z]+)_?\\d{0,4}p?(/playlist|.m3u8)", rtmp_live, 1);
//    qDebug() << __FUNCTION__ << key;
    return key;
}

QString GetDouYu::get_md5(QString str)
{
    QString md5Str = QCryptographicHash::hash(str.toUtf8(),QCryptographicHash::Md5).toHex();
    return md5Str;
}

QString GetDouYu::get_js()
{
   QString result = ComNetWork::search("(function ub98484234.*\\];)\nfunction", res, 1);
    if(result.isEmpty())
        return QString();
    result.replace(QRegExp("eval.*;\\}"), "strc;}");
    QString func_ub9 = result;
    QJSEngine jsEngine;
    jsEngine.evaluate(func_ub9);
    QJSValue func;
    func = jsEngine.globalObject().property("ub98484234");
    QString res_ = func.call().toString();
    if(res_.isEmpty())
        return QString();
    QString v = ComNetWork::search("v=(\\d+)", res_, 1);
    if(v.isEmpty()){
        return QString();
    }
    QString rb = get_md5(rid + did + t10 + v);
    QString func_sign = res_;
    func_sign.replace(QRegExp("return rt;\\}\\);?"), "return rt;}");
    func_sign.replace("(function (", "function sign(");
    func_sign.replace("CryptoJS.MD5(cb).toString()", "\"" + rb + "\"");
    QJSValue js2 = jsEngine.evaluate(func_sign);
    QJSValueList para;
    para << QJSValue(rid) << QJSValue(did) << QJSValue(t10);
    QJSValue func2;
    func2 = jsEngine.globalObject().property("sign");
    QString params = func2.call(para).toString();
    if(params.isEmpty())
        return QString();
    params += QString("&ver=219032101&rid=%1&rate=-1").arg(rid);
    QString url = "http://m.douyu.com/api/room/ratestream?" + params;
    QByteArray postRes = ComNetWork::post(url);
    if(postRes.isEmpty())
        return QString();
    QJsonObject postJson = QJsonDocument::fromJson(postRes).object();
    if(!postJson.contains("data")){
        return QString();
    }
    QJsonObject postData = postJson.value("data").toObject();
    if(postData.contains("settings")){
        QJsonArray rateJsonArray = postData.value("settings").toArray();
        foreach (auto tmpObj, rateJsonArray) {
            rateObjLst.append(tmpObj);
        }
    }
    QString key = ComNetWork::search("(\\d{1,8}[0-9a-zA-Z]+)_?\\d{0,4}p?(.m3u8|/playlist)", postRes, 1);
//    qDebug() << __FUNCTION__ << postRes;
//    qDebug() << __FUNCTION__ << key;
    return key;
}

void GetDouYu::run()
{
    getRealUrl();
}
