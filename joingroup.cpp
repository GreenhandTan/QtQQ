#include "joingroup.h"
#include "ui_joingroup.h"
#include <QDebug>

JoinGroup::JoinGroup(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::JoinGroup)
{
    ui->setupUi(this);
}

JoinGroup::~JoinGroup()
{
    delete ui;
}

void JoinGroup::recvJoinGroup()
{
    this->show();
}

void JoinGroup::closeJoinGroup()
{
    this->hide();
}

void JoinGroup::on_pushButton_clicked()
{
    QString groupname=ui->lineEdit->text();
    QString username=ui->lineEdit_2->text();
    groupWidget=new Udp_Weiget(0,username);
    groupWidget->setWindowTitle(groupname);
    groupWidget->show();
    closeJoinGroup();
}

//匿名加入功能
void JoinGroup::on_checkBox_stateChanged(int arg1)
{
    if(arg1 == 2){
        this->ui->lineEdit_2->setText("******");
    }else{
        this->ui->lineEdit_2->setText("");
    }
    this->ui->lineEdit_2->setDisabled(arg1);
}
