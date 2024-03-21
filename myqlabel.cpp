#include "myqlabel.h"
#include "ui_myqlabel.h"

myQLabel::myQLabel(QWidget *parent,QString username) :
    QWidget(parent),
    ui(new Ui::myQLabel)
{
    ui->setupUi(this);

    //随机头像
    int set_index_img = 0;
    QTime time_sand;
    time_sand= QTime::currentTime();//获取当前时间
    qsrand(time_sand.msec()+time_sand.second()*1000);
    set_index_img = qrand()%8;//在0-8中选出随机数
    QString ico_img_path=":/images/ico/user0.jpeg";
    ico_img_path = ":/images/ico/user" + QString::number(set_index_img, 10)  + ".jpeg";

    this->ui->label->setPixmap(ico_img_path);//设置头像
    this->ui->label_2->setText(username);//设置用户名
    this->ui->label_3->setText("这个人什么都没留下。");//设置个性签名
}

myQLabel::~myQLabel()
{
    delete ui;
}
