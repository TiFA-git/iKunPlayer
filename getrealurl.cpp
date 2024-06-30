#include "getrealurl.h"
#include <QApplication>
#include <QFile>
#include <QIODevice>
#include <QThreadPool>
#include <QThread>

GetRealUrl::GetRealUrl(QObject *parent) : QObject(parent)
{
    loadPlayLst();
    QThreadPool::globalInstance()->setMaxThreadCount(5);
}

GetRealUrl::~GetRealUrl()
{
    QThreadPool::globalInstance()->releaseThread();
    QThreadPool::globalInstance()->clear();
    QThreadPool::globalInstance()->~QThreadPool();
}

void GetRealUrl::getRealUrl()
{
//    qDebug() << "getrealurl";
    foreach (QStringList curLine, playLst) {
        QString rid = curLine.at(1);
        QString name = curLine.at(0);
        if(curLine.at(2) == "斗鱼"){
            GetDouYu *DYGeter = new GetDouYu(this);
            connect(DYGeter, &GetDouYu::sig_sendRes, this, &GetRealUrl::slots_convertUrl, Qt::DirectConnection);
            DYGeter->setRid(rid);
            DYGeter->setNick(name);
            DYGeter->setAutoDelete(true);
            QThreadPool::globalInstance()->start(DYGeter);
        }else if(curLine.at(2) == "虎牙"){
            GetHuYa *HYGeter =new GetHuYa(this);
            connect(HYGeter,&GetHuYa::sig_sendUrl, this, &GetRealUrl::slots_convertUrl, Qt::DirectConnection);
            HYGeter->setNick(name);
            HYGeter->setRid(rid);
            HYGeter->setAutoDelete(true);
            QThreadPool::globalInstance()->start(HYGeter);
        }
    }
    while (QThreadPool::globalInstance()->activeThreadCount() > 0) {
        qDebug() << QThreadPool::globalInstance()->activeThreadCount();
        QThread::sleep(1);
    }
    emit sig_getFinish();
}

void GetRealUrl::slots_convertUrl(QString name, QJsonObject obj, QString rid)
{
    emit sig_getSuccess(name, obj, rid);
}

void GetRealUrl::loadPlayLst()
{
//    QString path = QString("/Users/zx/qtWorkspace/QtAppProject-mpvDemoModify/bin/debug/playlist.csv");
    QString path = QApplication::applicationDirPath() + QString("/playlist.csv");
    QFile lstFile(path);
    if(!lstFile.open(QIODevice::ReadOnly)){
        return;
    }
    QTextStream txtStream(&lstFile);
    txtStream.setCodec("GBK");
    while(!txtStream.atEnd()){
        QString lineStr = txtStream.readLine();
        QStringList lineSplit = lineStr.simplified().split(",");
        if(lineSplit.size() < 3){
            continue;
        }
        playLst.append(lineSplit);
    }
    lstFile.close();
}
