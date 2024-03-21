#include <QApplication>
#include <QtWidgets>
#include <QTextCodec>
#include "login.h"
#include "user_list.h"
#include "adduser.h"
#include "addgroup.h"
#include "joingroup.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTextCodec::setCodecForLocale(QTextCodec::codecForLocale());
    Login w;
    w.show();
    user_list w1;
    AddUser w2;
    AddGroup w3;
    JoinGroup w4;
    void (Login::*usersignal)(QString userName) = &Login::showWidget;
    void (user_list::*userslot)(QString userName) = &user_list::recvWidget;
    void (AddUser::*addusersignal)(QString userName) = &AddUser::addUser;
    void (user_list::*adduserslot)(QString userName) = &user_list::addUser;
    QObject::connect(&w,usersignal,&w1,userslot);
    QObject::connect(&w1,SIGNAL(showAddUser()),&w2,SLOT(recvAddUser()));
    QObject::connect(&w2,addusersignal,&w1,adduserslot);
    QObject::connect(&w1,SIGNAL(showAddGroup()),&w3,SLOT(recvAddGroup()));
    QObject::connect(&w1,SIGNAL(showJoinGroup()),&w4,SLOT(recvJoinGroup()));

    //加载qss start
    QFile file(":/images/qss/black.qss");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString qss(file.readAll());
    a.setStyleSheet(qss);
    //加载qss end
    
    return a.exec();
}
