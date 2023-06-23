#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "controller.h"
#include "playerlistwidget.h"
#include "danmuclient.h"
#include "bullet.h"
#include "bulletpad.h"
#include "playerwidget.h"

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
#include <QVBoxLayout>
#include <QStackedLayout>
#include <QDebug>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_isFullScreen(false),
    isAllowDanMu(false),
    m_curBulletCnt(0),
    m_topLayerWidget(nullptr)
{
    ui->setupUi(this);
    setWindowTitle("iKunPlayer");

    initPlayer();   // 初始化播放器
    initControllerWidget(); //  全屏控制器
    initListWidget();   //  全屏播放列表
    initBulletClient();  // 创建弹幕服务器
    initBulletPad();    // 弹幕控制器
    initRealUrl();  //  获取直播流地址
}

MainWindow::~MainWindow()
{
    delete ui;
    delete hideCursor;
    delete onTimeUpdateUrls;
    delete urlGetter;
    delete loadThread;
}

void MainWindow::moveEvent(QMoveEvent * event)
{
    m_playerWidget->setTopLayerPos(m_playerWidget->mapToGlobal(QPoint(0, 0)));
    QMainWindow::moveEvent(event);
}

void MainWindow::onProcessFullScreen()
{    
    // arm下全屏mpv有概率崩溃，在全屏前先停止播放
    m_player->stop();
    if(m_isFullScreen){
        m_isFullScreen = false;
        setWindowFlags(Qt::Widget);
        ui->leftFrame->setHidden(false);
        ui->ctrlFrame->setHidden(false);
        playerListWidget->hide();
        showNormal();
    }else{
        m_isFullScreen = true;
        setWindowFlags(Qt::Window);
        ui->leftFrame->setHidden(true);
        ui->ctrlFrame->setHidden(true);
        showFullScreen();
    }
    m_player->play(m_curUrl);
}

void MainWindow::slot_recRes(QString nick, QJsonObject res, QString rid)
{
    if(!res.isEmpty()){
        m_curUrlObj = res;
        if(realUrlsMap.contains(nick)){
            realUrlsMap.remove(nick);
            m_nick2Rid.remove(nick);
        }
        realUrlsMap.insert(nick, m_curUrlObj);
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
    m_bulletPad->hide();
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

//        m_playerWidget->play(videoName); // 播放视频
//    }
//    else
//    {
//        m_isPlay = false;
//        ui->playPushButton->setText("播放");
//        ui->pausePushButton->setText("暂停");

//        m_playerWidget->slot_setProperty("pause", "no");
//        m_playerWidget->play(" "); // 播放空视频，代替停止播放
//    }


}

// 暂停/恢复视频
void MainWindow::on_pausePushButton_clicked()
{
//    // 有流在播放才可以暂停
//    if (!isPlay())
//    {
//        QMessageBox::warning(this, tr("警告对话框"),tr("有流在播放才可以暂停！"), QMessageBox::Abort);
//        return;
//    }
//    // 录制时不能暂停
//    if (isRecord())
//    {
//        QMessageBox::warning(this, tr("警告对话框"),tr("录制中不能暂停！"), QMessageBox::Abort);
//        return;
//    }

    // 获得mpv播放器的"暂停"状态
    QString pasued = m_player->getProperty("pause");
    // 根据"暂停"状态来选择暂停还是播放
    if(pasued == "no")
    {
        m_player->stop();
        ui->pausePushButton->setText("恢复");
        m_player->slot_setProperty("pause", "yes");
    }
    else if(pasued == "yes")
    {
        ui->pausePushButton->setText("暂停");
        m_player->slot_setProperty("pause", "no");
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
    QString pasued = m_player->getProperty("pause");
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

        m_player->slot_setProperty("stream-record", "./out.mp4");
    }
    else
    {
        // 停止录制
        m_isRecord = false;
        ui->recordPushButton->setText("录像");

        m_player->slot_setProperty("stream-record", " ");
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
    m_curUrlObj = realUrlsMap.value(clickName);
    ui->rateSelect->clear();
    playerListWidget->clearRate();
    foreach (QString rateName, m_curUrlObj.keys()) {
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
    QString url = m_curUrlObj.value(key).toString();
    m_player->play(url);
    m_curUrl = url;
}

void MainWindow::on_pushButton_2_clicked()
{
    onProcessFullScreen();
}

void MainWindow::slot_pause()
{
    // 获得mpv播放器的"暂停"状态
    QString pasued = m_player->getProperty("pause");
    // 根据"暂停"状态来选择暂停还是播放
    if(pasued == "no")
    {
        ui->pausePushButton->setText("恢复");
        m_player->slot_setProperty("pause", "yes");
    }
    else if(pasued == "yes")
    {
        ui->pausePushButton->setText("暂停");
        m_player->slot_setProperty("pause", "no");
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
        if(m_controllerWidget->isVisible())
            m_controllerWidget->hide();
        return;
    }
    if(m_controllerWidget->isVisible() != b){
        m_controllerWidget->setVisible(b);
    }
}

void MainWindow::slot_taggleLst()
{
    playerListWidget->setVisible(playerListWidget->isHidden());
}

void MainWindow::slot_receivedBullet(QString msg, QString name, QString board)
{
//    QString txt = (board.size() > 0) ? QString("[%1]%2：%3").arg(board, name, msg) : QString("%1：%2").arg(name, msg);
    QString txt = (board.size() > 0) ? QString("[%1]%2").arg(board, msg) : QString("%1").arg(msg);
    if(m_isFullScreen){
        if(m_curBulletCnt > m_maxBulletNum){
            return;
        }
        m_curBulletCnt++;
        Bullet *bullet = new Bullet(m_topLayerWidget);
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
    m_controllerWidget = new Controller(this);
    m_controllerWidget->move((QApplication::desktop()->width() - m_controllerWidget->width()) / 2,
                           QApplication::desktop()->height()- 200);
    connect(m_controllerWidget, &Controller::sig_sendCMD, m_player, &MpvPlayerHandler::slot_setProperty);
    connect(m_playerWidget, &PlayerWidget::sig_showCtrl, this, &MainWindow::slot_processCtrlShow);
    connect(m_controllerWidget, &Controller::sig_toggleList, this, &MainWindow::slot_taggleLst);
    connect(m_controllerWidget, &Controller::sig_toggleButtle, this, &MainWindow::slot_toggleBullet);
    connect(ui->checkBox, &QCheckBox::toggled, this, &MainWindow::slot_toggleBullet);  // 弹幕控制 勾选同步
    connect(m_controllerWidget, &Controller::sig_bulletSetting, this, &MainWindow::slot_showBulletPad);
}

void MainWindow::initRealUrl()
{

    loadThread = new QThread(this);
    urlGetter = new GetRealUrl;
    urlGetter->moveToThread(loadThread);
    connect(this, &MainWindow::sig_runGet, urlGetter, &GetRealUrl::getRealUrl);
    connect(urlGetter, &GetRealUrl::sig_getSuccess, this, &MainWindow::slot_recRes);
    connect(urlGetter, &GetRealUrl::sig_getFinish, this, &MainWindow::slot_updateUI);
    loadThread->start();

    m_isPlay = false;
    m_isRecord = false;

    onTimeUpdateUrls = new QTimer(this);
    onTimeUpdateUrls->setInterval(60000);
    connect(onTimeUpdateUrls, &QTimer::timeout, this, &MainWindow::slot_updateUrls);
    onTimeUpdateUrls->start();

    emit sig_runGet();  //  获取直播流
}

void MainWindow::on_checkBox_toggled(bool checked)
{
    slot_toggleBullet(checked);
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

void MainWindow::slot_toggleBullet(bool checked)
{
    toggleDanMu(checked);
    // 同步勾选
    QCheckBox *curCheckBox = qobject_cast<QCheckBox *>(sender());
    if(curCheckBox == ui->checkBox){        // 主页的钩子同步到pad
        m_controllerWidget->setCheckBoxState(checked);
    }else{
        ui->checkBox->setChecked(checked);
    }
}

void MainWindow::slot_showBulletPad()
{
    m_bulletPad->setVisible(m_bulletPad->isHidden());
}

void MainWindow::initPlayer()
{
    // 播放器抓手
    m_player = new MpvPlayerHandler(this);
    connect(m_player, &MpvPlayerHandler::mpv_palyEnd, this, &MainWindow::paltEnd_slot);
    m_player->slot_setProperty("input-cursor", "no");
    m_player->slot_setProperty("input-buildin-bindings", "yes");

    // 播放器界面
    m_playerWidget = new PlayerWidget(this);
    m_player->setWinID(m_playerWidget->winId());
    connect(m_playerWidget, &PlayerWidget::sig_pause, this, &MainWindow::slot_pause);
    connect(m_playerWidget, &PlayerWidget::send_setFullScreen, this, &MainWindow::onProcessFullScreen);
    m_topLayerWidget = m_playerWidget->topLayerWidget();

    //  添加到主界面
    QVBoxLayout *vb = new QVBoxLayout(this);
    vb->setMargin(0);
    vb->addWidget(m_playerWidget);
    ui->playerFrame->setLayout(vb);

}

