#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "controller.h"
#include "playerlistwidget.h"
#include "danmuclient.h"
#include "bullet.h"
#include "bulletpad.h"

#include <QRandomGenerator>
#include <QList>
#include <QJsonObject>
#include <QJsonArray>
#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QTimer>
#include <QThreadPool>
#include <QThread>
#include <QDesktopWidget>
#include <QDebug>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_isFullScreen(false),
    isAllowDanMu(false),
    m_curBulletCnt(0)
{
    ui->setupUi(this);
    setWindowTitle("iKunPlayer");


    initControllerWidget(); //  全屏控制器
    initListWidget();   //  全屏播放列表
    initBulletClient();  // 创建弹幕服务器
    initBulletPad();    // 弹幕控制器

    loadThread = new QThread(this);
    urlGetter = new GetRealUrl;
    urlGetter->moveToThread(loadThread);
    connect(this, &MainWindow::sig_runGet, urlGetter, &GetRealUrl::getRealUrl);
    connect(urlGetter, &GetRealUrl::sig_getSuccess, this, &MainWindow::slot_recRes);
    connect(urlGetter, &GetRealUrl::sig_getFinish, this, &MainWindow::slot_updateUI);
    loadThread->start();

    m_isPlay = false;
    m_isRecord = false;

    // 暂停
    connect(ui->playerWidget, &MpvPlayerWidget::sig_pause, this, &MainWindow::slot_pause);

    connect(ui->playerWidget, &MpvPlayerWidget::mpv_palyEnd, this, &MainWindow::paltEnd_slot);
    connect(ui->playerWidget, &MpvPlayerWidget::send_setFullScreen, this, &MainWindow::onProcessFullScreen);

    onTimeUpdateUrls = new QTimer(this);
    onTimeUpdateUrls->setInterval(60000);
    connect(onTimeUpdateUrls, &QTimer::timeout, this, &MainWindow::slot_updateUrls);
    onTimeUpdateUrls->start();

    ui->playerWidget->slot_setProperty("input-cursor", "no");
    ui->playerWidget->slot_setProperty("input-buildin-bindings", "yes");

    emit sig_runGet();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete hideCursor;
    delete onTimeUpdateUrls;
    delete urlGetter;
    delete loadThread;
}

void MainWindow::onProcessFullScreen()
{    

    if(m_isFullScreen){
        m_isFullScreen = false;
        setWindowFlags(Qt::Widget);
        showNormal();
        ui->leftFrame->setHidden(false);
        ui->ctrlFrame->setHidden(false);
        playerListWidget->hide();
    }else{
        m_isFullScreen = true;
        setWindowFlags(Qt::Window);
        showFullScreen();
        ui->leftFrame->setHidden(true);
        ui->ctrlFrame->setHidden(true);
    }
}

void MainWindow::slot_recRes(QString nick, QJsonObject res, QString rid)
{
    if(!res.isEmpty()){
        curUrlObj = res;
        if(realUrlsMap.contains(nick)){
            realUrlsMap.remove(nick);
            m_nick2Rid.remove(nick);
        }
        realUrlsMap.insert(nick, curUrlObj);
        m_nick2Rid.insert(nick, rid);
    }
}


// 判断是否在播放
bool MainWindow::isPlay()
{
    return m_isPlay;
}

// 判断是否在录制
bool MainWindow::isRecord()
{
    return m_isRecord;
}

void MainWindow::initListWidget()
{
    playerListWidget = new PlayerListWidget(this);
    playerListWidget->setFixedHeight(QApplication::desktop()->height());
    playerListWidget->move(0, 0);
    playerListWidget->hide();
    connect(playerListWidget, &PlayerListWidget::sig_ItemClick, this, &MainWindow::on_listWidget_itemClicked);
}

void MainWindow::initBulletClient()
{
    m_bulletThread = new QThread(this);
    m_bulletClient = new DanMuClient(this);
    // 登陆信号
    connect(this, &MainWindow::sig_loginBulletClient, m_bulletClient, &DanMuClient::slot_loginServer);
    connect(m_bulletClient, &DanMuClient::sig_lunchBullet, this, &MainWindow::slot_receivedBullet, Qt::QueuedConnection);
    // 登出信号
    connect(this, &MainWindow::sig_stopDanMuClient, m_bulletClient, &DanMuClient::slot_logoutSever);
    m_bulletClient->moveToThread(m_bulletThread);
    m_bulletThread->start();
}

void MainWindow::toggleDanMu(bool b)
{
    isAllowDanMu = b;
    if(b){
        emit sig_loginBulletClient(m_curRid);  // 连接弹幕
    }else{
        bool curState = m_bulletClient->getBulletState();
        qDebug() << curState;
        if(curState){
            emit sig_stopDanMuClient();  // 关闭弹幕连接
        }
        // 清除屏幕上的弹幕
        emit sig_clearBullet();
    }
}

void MainWindow::initBulletPad()
{
    m_bulletPad = new BulletPad(this);
    m_maxBulletNum = m_bulletPad->getBulletNum();
    m_fontSize = m_bulletPad->getFontSize();
    m_rowNum = m_bulletPad->getBulletRow();
    connect(m_bulletPad, &BulletPad::sig_bulletRowCnt, this, &MainWindow::slot_BulletRowCnt);
    connect(m_bulletPad, &BulletPad::sig_fontSizePixel, this, &MainWindow::slot_BulletSize);
    connect(m_bulletPad, &BulletPad::sig_maxBulletNum, this, &MainWindow::slot_maxBulletNum);
}

void MainWindow::slot_updateUrls()
{
    realUrlsMap.clear();
    emit sig_runGet();
}


// 播放/停止
void MainWindow::on_playPushButton_clicked()
{

//    QString videoName = "http://al.flv.huya.com/src/1394575534-1394575534-5989656310331736064-2789274524-10057-A-0-1.flv?wsSecret=d17a56fc88ef412df81e8e9b8b6db032&wsTime=63df1667&fm=RFdxOEJjSjNoNkRKdDZUWV8kMF8kMV8kMl8kMw%3D%3D&txyp=o%3Aqzeic1%3B&fs=bgct&sphdcdn=al_7-tx_3-js_3-ws_7-bd_2-hw_2&sphdDC=huya&sphd=264_*-265_*&exsphd=264_500,264_2000,264_4000,&t=103";
//    //QString videoName = "rtsp://wowzaec2demo.streamlock.net/vod/mp4:BigBuckBunny_115k.mov";
//    // 选择是否播放视频
//    if (!isPlay())
//    {
//        m_isPlay = true;
//        ui->playPushButton->setText("停止");

//        ui->playerWidget->play(videoName); // 播放视频
//    }
//    else
//    {
//        m_isPlay = false;
//        ui->playPushButton->setText("播放");
//        ui->pausePushButton->setText("暂停");

//        ui->playerWidget->slot_setProperty("pause", "no");
//        ui->playerWidget->play(" "); // 播放空视频，代替停止播放
//    }


}

// 暂停/恢复视频
void MainWindow::on_pausePushButton_clicked()
{
    // 有流在播放才可以暂停
    if (!isPlay())
    {
        QMessageBox::warning(this, tr("警告对话框"),tr("有流在播放才可以暂停！"), QMessageBox::Abort);
        return;
    }
    // 录制时不能暂停
    if (isRecord())
    {
        QMessageBox::warning(this, tr("警告对话框"),tr("录制中不能暂停！"), QMessageBox::Abort);
        return;
    }

    // 获得mpv播放器的"暂停"状态
    QString pasued = ui->playerWidget->getProperty("pause");
    // 根据"暂停"状态来选择暂停还是播放
    if(pasued == "no")
    {
        ui->pausePushButton->setText("恢复");
        ui->playerWidget->slot_setProperty("pause", "yes");
    }
    else if(pasued == "yes")
    {
        ui->pausePushButton->setText("暂停");
        ui->playerWidget->slot_setProperty("pause", "no");
    }
}

void MainWindow::on_recordPushButton_clicked()
{
    // 有流播放时，才能录制
    if (!isPlay())
    {
        QMessageBox::warning(this, tr("警告对话框"),tr("有流播放时，才能录制！"), QMessageBox::Abort);
        return;
    }

    // 暂停时不能录制
    QString pasued = ui->playerWidget->getProperty("pause");
    if (pasued == "yes")
    {
        QMessageBox::warning(this, tr("警告对话框"),tr("暂停时不能录制！"), QMessageBox::Abort);
        return;
    }

    if (!isRecord())
    {
        // 开始录制视频
        m_isRecord = true;
        ui->recordPushButton->setText("录像中");

        ui->playerWidget->slot_setProperty("stream-record", "./out.mp4");
    }
    else
    {
        // 停止录制
        m_isRecord = false;
        ui->recordPushButton->setText("录像");

        ui->playerWidget->slot_setProperty("stream-record", " ");
    }
}


// 接受播放结束信号的槽函数
void MainWindow::paltEnd_slot()
{
    m_isPlay = false;
    ui->playPushButton->setText("播放");
}

void MainWindow::on_listWidget_itemClicked(QListWidgetItem *item)
{
    QString clickName = item->text();
    curUrlObj = realUrlsMap.value(clickName);
    ui->rateSelect->clear();
    playerListWidget->clearRate();
    foreach (QString rateName, curUrlObj.keys()) {
        ui->rateSelect->addItem(rateName);
        playerListWidget->addRate(rateName);
    }
    // 切换弹幕服务器
    m_curRid = m_nick2Rid.value(clickName);
    if(isAllowDanMu){
        emit sig_loginBulletClient(m_curRid);
    }
}

void MainWindow::on_pushButton_clicked()
{
    emit sig_runGet();
}

void MainWindow::on_rateSelect_currentTextChanged(const QString &key)
{
    QString url = curUrlObj.value(key).toString();
    ui->playerWidget->play(url);
}

void MainWindow::on_pushButton_2_clicked()
{
    onProcessFullScreen();
//    ui->playerWidget->setProperty("input-keylist", "");
}

void MainWindow::slot_pause()
{
    // 获得mpv播放器的"暂停"状态
    QString pasued = ui->playerWidget->getProperty("pause");
    // 根据"暂停"状态来选择暂停还是播放
    if(pasued == "no")
    {
        ui->pausePushButton->setText("恢复");
        ui->playerWidget->slot_setProperty("pause", "yes");
    }
    else if(pasued == "yes")
    {
        ui->pausePushButton->setText("暂停");
        ui->playerWidget->slot_setProperty("pause", "no");
    }
}

void MainWindow::slot_updateUI()
{
    bool b =false;
    QList<QString> newLst = realUrlsMap.keys();
    QList<QString> oldLst = realUrlsMapUI.keys();
    if(newLst.size() == oldLst.size()){
        foreach(QString tmp, newLst){
            if(!oldLst.contains(tmp)){
                b = true;
                break;
            }
        }
    }else{
        b = true;
    }
    if(b){
        realUrlsMapUI = realUrlsMap;
        ui->listWidget->clear();
        playerListWidget->clearLst();
        foreach(QString tmp2, newLst){
            ui->listWidget->addItem(tmp2);
            playerListWidget->addItem(tmp2);
        }
    }
}

void MainWindow::slot_processCtrlShow(bool b)
{
    if(m_isFullScreen == false){
        if(controllerWidget->isVisible())
            controllerWidget->hide();
        return;
    }
    if(controllerWidget->isVisible() != b){
        controllerWidget->setVisible(b);
    }
}

void MainWindow::slot_taggleLst()
{
    playerListWidget->setVisible(playerListWidget->isHidden());
}

void MainWindow::slot_receivedBullet(QString msg, QString name, QString board)
{
    QString txt = (board.size() > 0) ? QString("[%1]%2：%3").arg(board, name, msg) : QString("%1：%2").arg(name, msg);
    if(m_isFullScreen){
        if(m_curBulletCnt > m_maxBulletNum){
            return;
        }
        m_curBulletCnt++;
        Bullet *bullet = new Bullet(this);
        connect(bullet, SIGNAL(sig_accurated(QObject*)), this, SLOT(slot_accurated(QObject*)));
        connect(this, &MainWindow::sig_clearBullet, bullet, &Bullet::slot_destroyBullet);
        bullet->setFontSize(m_fontSize);
        int lineIdx = QRandomGenerator::global()->bounded(m_rowNum);  // 随机第几行显示
        bullet->shoot(txt, lineIdx);
    }else {
        ui->textEdit_bulletScreen->append("\n" + txt);
    }
}

void MainWindow::slot_accurated(QObject * bullet)
{
    if(bullet){
        delete bullet;
        bullet = nullptr;
        m_curBulletCnt--;
    }
}

void MainWindow::initControllerWidget()
{
    controllerWidget = new Controller(this);
    controllerWidget->move((QApplication::desktop()->width() - controllerWidget->width()) / 2,
                           QApplication::desktop()->height()- 200);
    connect(controllerWidget, &Controller::sig_sendCMD, ui->playerWidget, &MpvPlayerWidget::slot_setProperty);
    connect(ui->playerWidget, &MpvPlayerWidget::sig_showCtrl, this, &MainWindow::slot_processCtrlShow);
    connect(controllerWidget, &Controller::sig_taggleList, this, &MainWindow::slot_taggleLst);
}

void MainWindow::on_checkBox_toggled(bool checked)
{
    isAllowDanMu = checked;
    toggleDanMu(isAllowDanMu);
    if(checked){
        m_bulletPad->show();
    }else{
        m_bulletPad->hide();
    }
}

void MainWindow::slot_maxBulletNum(int value)
{
    qDebug() << "num " << value;
    m_maxBulletNum = value;
}

void MainWindow::slot_BulletRowCnt(int value)
{
    qDebug() << "row " << value;
    m_rowNum = value;
}

void MainWindow::slot_BulletSize(int value)
{
    qDebug() << "size " << value;
    m_fontSize = value;
}

