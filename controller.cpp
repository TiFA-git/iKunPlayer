#include "controller.h"
#include "ui_controller.h"
#include <QDebug>

Controller::Controller(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Controller)
{
    ui->setupUi(this);
}

Controller::~Controller()
{
    delete ui;
}

void Controller::setCheckBoxState(bool checked)
{
    ui->checkBox->setChecked(checked);
}

void Controller::on_pushButton_send_clicked()
{
    qDebug() << __FUNCTION__ << ui->lineEdit_cmd->text();
    QString cmd = ui->lineEdit_cmd->text();
    QString args = ui->lineEdit_args->text();
    emit sig_sendCMD(cmd, args);
}


void Controller::on_pushButton_playlist_clicked()
{
    emit sig_toggleList();
}


void Controller::on_checkBox_toggled(bool checked)
{
    emit sig_toggleButtle(checked);
}


void Controller::on_pushButton_padSetting_clicked()
{
    emit sig_bulletSetting();
}

