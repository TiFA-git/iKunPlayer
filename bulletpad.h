#ifndef BULLETPAD_H
#define BULLETPAD_H

#include <QDialog>

namespace Ui {
class BulletPad;
}

class BulletPad : public QDialog
{
    Q_OBJECT

public:
    explicit BulletPad(QWidget *parent = nullptr);
    ~BulletPad();
    int getFontSize();
    int getBulletNum();
    int getBulletRow();

signals:
    void sig_maxBulletNum(int);
    void sig_fontSizePixel(int);
    void sig_bulletRowCnt(int);

private slots:
    void on_pushButton_clicked();

    void on_Slider_num_valueChanged(int value);

    void on_Slider_size_valueChanged(int value);

    void on_Slider_lineCnt_valueChanged(int value);

private:
    Ui::BulletPad *ui;
};

#endif // BULLETPAD_H
