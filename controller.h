#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QWidget>

namespace Ui {
class Controller;
}

class Controller : public QWidget
{
    Q_OBJECT

public:
    explicit Controller(QWidget *parent = nullptr);
    ~Controller();

    void setCheckBoxState(bool);

signals:
    void sig_sendCMD(QString cmd, QString msg);
    void sig_toggleList();
    void sig_bulletSetting();
    void sig_toggleButtle(bool);


private slots:
    void on_pushButton_send_clicked();

    void on_pushButton_playlist_clicked();

    void on_checkBox_toggled(bool checked);

    void on_pushButton_padSetting_clicked();

private:
    Ui::Controller *ui;
};

#endif // CONTROLLER_H
