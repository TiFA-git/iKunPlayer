#ifndef PLAYERLISTWIDGET_H
#define PLAYERLISTWIDGET_H

#include <QWidget>
#include <QListWidgetItem>

namespace Ui {
class PlayerListWidget;
}

class PlayerListWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PlayerListWidget(QWidget *parent = nullptr);
    ~PlayerListWidget();

    void clearLst();
    void addItem(QString);
    void clearRate();
    void addRate(QString);

signals:
    void sig_ItemClick(QListWidgetItem *item);

protected:
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);

private slots:
    void on_listWidget_itemClicked(QListWidgetItem *item);

private:
    Ui::PlayerListWidget *ui;
};

#endif // PLAYERLISTWIDGET_H
