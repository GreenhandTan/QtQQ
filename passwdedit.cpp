#include "passwdedit.h"
#include "ui_passwdedit.h"

#include <QMessageBox>
#include <QDebug>

passwdEdit::passwdEdit(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::passwdEdit)
{
    ui->setupUi(this);
    init();
}

passwdEdit::~passwdEdit()
{
    delete ui;
}

void passwdEdit::init()
{
    setWindowTitle(tr("修改密码"));
    connect(ui->btn_cancel,SIGNAL(clicked()),this,SLOT(close()));
}

void passwdEdit::setLogin(Login *m)
{
    if(m != NULL)
    {
        login = m;
    }
}

void passwdEdit::on_btn_ok_clicked()
{
    QString oldPasswd = ui->lineEdit_passwd->text();
    QString newPasswd = ui->lineEdit_newPasswd->text();
    QString passwdOk = ui->lineEdit_passwdOk->text();

    if(oldPasswd.isEmpty() || newPasswd.isEmpty() || passwdOk.isEmpty())
    {
        QMessageBox::warning(this,tr("警告"),tr("密码为空！"));
    }
    else
    {
        if(oldPasswd == login->user_info_stu.passwd)
        {
            if(oldPasswd == newPasswd)
            {
                QMessageBox::warning(this,tr("警告"),tr("新密码与旧密码一样！"));
                return;
            }
            else
            {
                if(newPasswd != passwdOk)
                {
                    QMessageBox::information(this,tr("提示"),tr("密码修改失败！"));
                }
                else
                {
                    //只获取信息不对数据库操作
                    login->user_info_stu.passwd = newPasswd;
                    qDebug()<<"新密码："<<login->user_info_stu.passwd;
                    //QMessageBox::information(this,tr("提示"),tr("密码修改成功！"));
                    this->close();
                }
            }
        }
        else
        {
            QMessageBox::warning(this,tr("警告"),tr("旧密码输入错误！"));
        }
    }
}
