#ifndef USER_LIST_H
#define USER_LIST_H
#include "udp_weiget.h"
#include "myqlabel.h"
#include <QWidget>
#include <QToolButton>
#include <QMouseEvent>
#include <QToolBox>
#include <QLabel>
#include <QTime>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QMediaPlayer>
#include <QSound>
#include <QTcpServer>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QString>
#include <QDebug>
#include <QApplication>
#include <QPainter>
#include <QMessageBox>
#include <QListWidgetItem>

#include "tcp_weiget.h"

namespace Ui {
class user_list;
}

class user_list : public QWidget
{
    Q_OBJECT

public:
    explicit user_list(QWidget *parent = nullptr);
    ~user_list();
    QMediaPlayer *startSound;  //创建一个音乐播放器
    QSound *addUserSound;
    Tcp_Weiget *chatWidget;//对应聊天窗口对象的指针

signals:
    void showAddUser();
    void showAddGroup();
    void showJoinGroup();
    void showJoinChat();

public slots:
    void recvWidget();
    void recvWidget(QString userName);
    void addUser();
    void addUser(QString username);//添加好友
    void onItemDoubleClicked(QListWidgetItem *item);

private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();

private:
    Ui::user_list *ui;
    QTime time_sand;
    QSqlDatabase db2;
};

#endif // USER_LIST_H
