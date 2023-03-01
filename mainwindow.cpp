#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QList>
#include <QJsonObject>
#include <QJsonArray>
#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QTimer>
#include <QThreadPool>
#include <QThread>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    isFullScreen(false)
{
    ui->setupUi(this);
    setWindowTitle("IKUNPlayer");
    urlGetter = new GetRealUrl;
    QThread *loadThread = new QThread(this);
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

//    ui->playerWidget->setProperty("input-x11-keyboard", "yes");
//    ui->playerWidget->setProperty("input-default-bindings", "yes");
//    ui->playerWidget->setProperty("input-media-keys", "yes");
    ui->playerWidget->setProperty("input-cursor", "no");
    ui->playerWidget->setProperty("input-buildin-bindings", "yes");


    emit sig_runGet();
//    setFocusPolicy(Qt::StrongFocus);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onProcessFullScreen()
{    
    if(isFullScreen){
        isFullScreen = false;
        setWindowFlags(Qt::Widget);
        showNormal();
        ui->leftFrame->setHidden(false);
        ui->ctrlFrame->setHidden(false);
    }else{
        isFullScreen = true;
        setWindowFlags(Qt::Window);
        showFullScreen();
        ui->leftFrame->setHidden(true);
        ui->ctrlFrame->setHidden(true);
    }
}

void MainWindow::slot_recRes(QString nick, QJsonObject res)
{
    qDebug() << nick;
    if(!res.isEmpty()){
        curUrlObj = res;
        if(realUrlsMap.contains(nick)){
            realUrlsMap.remove(nick);
        }
        realUrlsMap.insert(nick, curUrlObj);
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

void MainWindow::slot_updateUrls()
{
    realUrlsMap.clear();
    emit sig_runGet();
}


// 播放/停止
void MainWindow::on_playPushButton_clicked()
{
    QString videoName = "http://al.flv.huya.com/src/1394575534-1394575534-5989656310331736064-2789274524-10057-A-0-1.flv?wsSecret=d17a56fc88ef412df81e8e9b8b6db032&wsTime=63df1667&fm=RFdxOEJjSjNoNkRKdDZUWV8kMF8kMV8kMl8kMw%3D%3D&txyp=o%3Aqzeic1%3B&fs=bgct&sphdcdn=al_7-tx_3-js_3-ws_7-bd_2-hw_2&sphdDC=huya&sphd=264_*-265_*&exsphd=264_500,264_2000,264_4000,&t=103";
    //QString videoName = "rtsp://wowzaec2demo.streamlock.net/vod/mp4:BigBuckBunny_115k.mov";

    // 选择是否播放视频
    if (!isPlay())
    {
        m_isPlay = true;
        ui->playPushButton->setText("停止");

        ui->playerWidget->play(videoName); // 播放视频
    }
    else
    {
        m_isPlay = false;
        ui->playPushButton->setText("播放");
        ui->pausePushButton->setText("暂停");

        ui->playerWidget->setProperty("pause", "no");
        ui->playerWidget->play(" "); // 播放空视频，代替停止播放
    }
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
        ui->playerWidget->setProperty("pause", "yes");
    }
    else if(pasued == "yes")
    {
        ui->pausePushButton->setText("暂停");
        ui->playerWidget->setProperty("pause", "no");
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

        ui->playerWidget->setProperty("stream-record", "./out.mp4");
    }
    else
    {
        // 停止录制
        m_isRecord = false;
        ui->recordPushButton->setText("录像");

        ui->playerWidget->setProperty("stream-record", " ");
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
    foreach (QString rateName, curUrlObj.keys()) {
        ui->rateSelect->addItem(rateName);
    }
}

void MainWindow::on_pushButton_clicked()
{
    emit sig_runGet();
}

void MainWindow::on_rateSelect_currentTextChanged(const QString &key)
{
    QString url = curUrlObj.value(key).toString();
    qDebug() << url;
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
        ui->playerWidget->setProperty("pause", "yes");
    }
    else if(pasued == "yes")
    {
        ui->pausePushButton->setText("暂停");
        ui->playerWidget->setProperty("pause", "no");
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
        foreach(QString tmp2, newLst){
            ui->listWidget->addItem(tmp2);
        }
    }
}
