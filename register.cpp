#include "register.h"
#include "ui_register.h"

Register::Register(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Register)
{
    ui->setupUi(this);

    setWindowTitle(tr("注册"));
}

Register::~Register()
{
    delete ui;
}

void Register::setParent(Login *dialog)
{
    if(dialog != NULL){
        pWidget = dialog;
    }
}


//注册函数.仅仅获取相关值，不做检测和数据库操作
void Register::on_registerCheckButton_clicked()
{
    pWidget->user_info_stu.userName = ui->lineEditName->text();
    pWidget->user_info_stu.passwd = ui->lineEditPassword->text();
    pWidget->user_info_stu.email = ui->lineEditEmail->text();
    this->close();
}
