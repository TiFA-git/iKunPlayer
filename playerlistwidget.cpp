#include "playerlistwidget.h"
#include "ui_playerlistwidget.h"

PlayerListWidget::PlayerListWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlayerListWidget)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);
}

PlayerListWidget::~PlayerListWidget()
{
    delete ui;
}

void PlayerListWidget::clearLst()
{
    ui->listWidget->clear();
}

void PlayerListWidget::addItem(QString str)
{
    ui->listWidget->addItem(str);
}

void PlayerListWidget::clearRate()
{
    ui->comboBox->clear();
}

void PlayerListWidget::addRate(QString rate_str)
{
    ui->comboBox->addItem(rate_str);
}

void PlayerListWidget::enterEvent(QEvent *event)
{
    setCursor(Qt::ArrowCursor);
    event->accept();
}

void PlayerListWidget::leaveEvent(QEvent *event)
{
    this->hide();
    event->accept();
}

void PlayerListWidget::on_listWidget_itemClicked(QListWidgetItem *item)
{
    emit sig_ItemClick(item);
}

