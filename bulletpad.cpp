#include "bulletpad.h"
#include "ui_bulletpad.h"

#include <QDebug>

BulletPad::BulletPad(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BulletPad)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlag(Qt::FramelessWindowHint);
}

BulletPad::~BulletPad()
{
    delete ui;
}

int BulletPad::getFontSize()
{
    return ui->Slider_size->value();
}

int BulletPad::getBulletNum()
{
    return ui->Slider_num->value();
}

int BulletPad::getBulletRow()
{
    return ui->Slider_lineCnt->value();
}

void BulletPad::on_pushButton_clicked()
{
    this->setVisible(this->isHidden());
}


void BulletPad::on_Slider_num_valueChanged(int value)
{
    emit sig_maxBulletNum(value);
}


void BulletPad::on_Slider_size_valueChanged(int value)
{
    emit sig_fontSizePixel(value);
}


void BulletPad::on_Slider_lineCnt_valueChanged(int value)
{
    emit sig_bulletRowCnt(value);
}

