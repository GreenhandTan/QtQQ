#include "addgroup.h"
#include "ui_addgroup.h"
#include <QString>
#include <QDebug>
#include "udp_weiget.h"
#include <QTime>
#include "user_list.h"

AddGroup::AddGroup(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AddGroup)
{
    ui->setupUi(this);
}

AddGroup::~AddGroup()
{
    delete ui;
}

void AddGroup::recvAddGroup()
{
    this->show();
}

void AddGroup::closeAddGroup()
{
    this->hide();
}

void AddGroup::on_pushButton_clicked()
{
    QString groupname=ui->lineEdit->text();
    QString username=ui->lineEdit_2->text();
    groupWidget=new Udp_Weiget(0,username);
    groupWidget->setWindowTitle(groupname);
    groupWidget->show();
    closeAddGroup();
}
