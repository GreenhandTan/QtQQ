#include "adduser.h"
#include "ui_adduser.h"
#include <QListWidgetItem>
#include <QDebug>

AddUser::AddUser(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AddUser)
{
    ui->setupUi(this);
}

AddUser::~AddUser()
{
    delete ui;
}

void AddUser::recvAddUser()
{
    this->show();
}

void AddUser::closeAddUser()
{
    this->ui->lineEdit->setText("");
    this->hide();
}

void AddUser::on_pushButton_clicked()
{
    QString username = this->ui->lineEdit->text();
    emit addUser(username);
    closeAddUser();
}
