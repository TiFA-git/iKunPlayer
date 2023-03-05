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

signals:
    void sig_sendCMD(QString, QString);


private slots:
    void on_pushButton_send_clicked();

private:
    Ui::Controller *ui;
};

#endif // CONTROLLER_H
