#include "user_list.h"
#include "ui_user_list.h"

user_list::user_list(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::user_list)
{
    //初始化数据库连接
    db2 = QSqlDatabase::addDatabase("QSQLITE","db2");
    db2.setDatabaseName("database.db");
    ui->setupUi(this);

    addUserSound = new QSound(":/sounds/keke.wav", this);

    connect(ui->listWidget,&QListWidget::itemDoubleClicked,this,&user_list::onItemDoubleClicked);
    qDebug()<<"初始化绑定好友成功";
}

user_list::~user_list()
{
    delete ui;
}
//打开聊天框
void user_list::onItemDoubleClicked(QListWidgetItem *item)
{
    chatWidget=new Tcp_Weiget(0,item->text(),this->ui->label_3->text());
    chatWidget->setWindowTitle(item->text());
    chatWidget->show();
}

void user_list::recvWidget(){}
void user_list::recvWidget(QString userName)
{
    this->show(); //显示本界面

    //随机头像
    int set_index_img = 0;
    QTime time_sand;
    time_sand= QTime::currentTime();//获取当前时间
    qsrand(time_sand.msec()+time_sand.second()*1000);
    set_index_img = qrand()%8;//在0-8中选出随机数
    QString ico_img_path=":/images/ico/user0.jpeg";
    ico_img_path = ":/images/ico/user" + QString::number(set_index_img, 10)  + ".jpeg";

    this->ui->label_3->setText(userName);
    this->ui->label->setPixmap(ico_img_path);
    //数据库查询好友并自动添加
    QSqlQuery query(db2);
    if(!db2.open())
    {
        qDebug() << "database open fail regist!";
    }else{
        QString friend_id = "";
        query.exec(QString("select * from userInfo where name = '%1'").arg(userName));
        while (query.next())
        {
            friend_id = query.value(3).toString();
        }
        QStringList MyFriendList = friend_id.split(QLatin1Char(' '));

        if(MyFriendList.size() >= 1 && MyFriendList[0] != ""){
            //存在好友
            foreach (QString str, MyFriendList) {
                QString username = "";
                query.exec(QString("select * from userInfo where id = %1").arg(str));
                while (query.next())
                {
                    username = query.value(0).toString();
                }
                myQLabel * ico = new myQLabel(this,username);
                QListWidgetItem *aItem = new QListWidgetItem(ui->listWidget);
                aItem->setSizeHint(QSize(275,60));
                aItem->setText(username);
                ui->listWidget->addItem(aItem);
                ui->listWidget->setItemWidget(aItem,ico);
                qDebug()<<"我的好友:::"<<username<<"添加成功！";
//                connect(ui->listWidget,&QListWidget::itemDoubleClicked,this,&user_list::onItemDoubleClicked);
//                qDebug()<<"绑定好友成功";
            }
        }else{
            //不存在好友
            qDebug()<<"登录初始化：：：不存在好友！";
        }
    }
    db2.close();
}
void user_list::addUser(){}
void user_list::addUser(QString username)
{
    QSqlQuery query(db2);
    QString myname = this->ui->label_3->text();//获取自己名字

    if(!db2.open())
    {
        qDebug() << "database open fail regist!";
    }
    else
    {
        QString friend_id = "";
        QString hisfriend_id = "";
        QString id = "";
        QString his_id = "";

        //查待加好友信息
        query.exec(QString("select * from userInfo where name = '%1'").arg(username));
        while(query.next())
        {
            //拿他的好友id
            hisfriend_id = query.value(3).toString();
            //拿待加好友的id
            his_id = query.value(4).toString();
        }
        if(his_id == "" || username == myname){
            QMessageBox::warning(this,tr("警告"),tr("用户异常，添加失败！"));
            qDebug()<<"用户不存在";
        }else{
            //用户存在,判断用户是否已在好友位
            query.exec(QString("select * from userInfo where name = '%1'").arg(myname));
            while(query.next())
            {
                //拿到自己的好友位id和id
                friend_id = query.value(3).toString();
                id = query.value(4).toString();
            }
            QStringList MyFriendList = friend_id.split(QLatin1Char(' '));

            if(MyFriendList.size() >= 1 && MyFriendList[0] != ""){
                //存在好友
                bool add_Flag = true;
                foreach (QString str, MyFriendList) {
                    QString feriend_name = "";
                    query.exec(QString("select * from userInfo where id = %1").arg(str));
                    while(query.next())
                    {
                        feriend_name = query.value(0).toString();
                    }
                    if(username == feriend_name){
                        add_Flag = false;
                    }
                }
                if(add_Flag){
                    qDebug()<<"不是好友！可以添加";
                    myQLabel * ico = new myQLabel(this,username);
                    QListWidgetItem *aItem = new QListWidgetItem(ui->listWidget);
                    aItem->setSizeHint(QSize(275,60));
                    ui->listWidget->addItem(aItem);
                    ui->listWidget->setItemWidget(aItem,ico);
                    qDebug()<<username<<"添加成功！";
//                    connect(ui->listWidget,&QListWidget::itemDoubleClicked,this,&user_list::onItemDoubleClicked);
//                    qDebug()<<"绑定好友成功";
                    addUserSound->play();
                    //更新我的数据库
                    QString myNewfriend_id = friend_id + " " + his_id;
                    query.prepare("UPDATE userInfo SET friend_id = :friend_id WHERE name = :name");
                    query.bindValue(":friend_id",myNewfriend_id);
                    query.bindValue(":name",myname);
                    if(!query.exec())
                    {
                        qDebug() << "更新失败!";
                    }
                    else
                    {
                        qDebug() << "更新成功！";
                        query.exec("select * from userInfo");
                        while (query.next())
                        {
                            QString userName = query.value(0).toString();
                            QString passwd = query.value(1).toString();
                            QString email = query.value(2).toString();
                            QString friend_id = query.value(3).toString();
                            QString id = query.value(4).toString();
                            qDebug() << "userName:::"<< userName << "passwd:::" << passwd<<"email:::"<<email<<"id:::"<<id<<"friend_id:::"<<friend_id;
                        }
                    }
                    //更新他的数据库,先判断他的好友是否为空
                    QString HisNewfriend_id = "";
                    if(hisfriend_id == ""){
                        HisNewfriend_id = id;
                    }else{
                        HisNewfriend_id = hisfriend_id + " " + id;
                    }
                    query.prepare("UPDATE userInfo SET friend_id = :friend_id WHERE name = :name");
                    query.bindValue(":friend_id",HisNewfriend_id);
                    query.bindValue(":name",username);
                    if(!query.exec())
                    {
                        qDebug() << "更新失败!";
                    }
                    else
                    {
                        qDebug() << "更新成功！";
                        query.exec("select * from userInfo");
                        while (query.next())
                        {
                            QString userName = query.value(0).toString();
                            QString passwd = query.value(1).toString();
                            QString email = query.value(2).toString();
                            QString friend_id = query.value(3).toString();
                            QString id = query.value(4).toString();
                            qDebug() << "userName:::"<< userName << "passwd:::" << passwd<<"email:::"<<email<<"id:::"<<id<<"friend_id:::"<<friend_id;
                        }
                    }
                }else{
                    QMessageBox::warning(this,tr("警告"),tr("用户异常，添加失败！"));
                }
            }else{
                //不是好友
                qDebug()<<"不是好友！可以添加";
                myQLabel * ico = new myQLabel(this,username);
                QListWidgetItem *aItem = new QListWidgetItem(ui->listWidget);
                aItem->setSizeHint(QSize(275,60));
                ui->listWidget->addItem(aItem);
                ui->listWidget->setItemWidget(aItem,ico);
                qDebug()<<username<<"添加成功！";
//                connect(ui->listWidget,&QListWidget::itemDoubleClicked,this,&user_list::onItemDoubleClicked);
//                qDebug()<<"绑定好友成功";
                addUserSound->play();
                //更新我的数据库,先判断好友是否为空
                QString myNewfriend_id = "";
                if(friend_id == ""){
                    myNewfriend_id = his_id;
                }else{
                    myNewfriend_id = friend_id + " " + his_id;
                }
                query.prepare("UPDATE userInfo SET friend_id = :friend_id WHERE name = :name");
                query.bindValue(":friend_id",myNewfriend_id);
                query.bindValue(":name",myname);
                if(!query.exec())
                {
                    qDebug() << "更新失败!";
                }
                else
                {
                    qDebug() << "更新成功！";
                    query.exec("select * from userInfo");
                    while (query.next())
                    {
                        QString userName = query.value(0).toString();
                        QString passwd = query.value(1).toString();
                        QString email = query.value(2).toString();
                        QString friend_id = query.value(3).toString();
                        QString id = query.value(4).toString();
                        qDebug() << "userName:::"<< userName << "passwd:::" << passwd<<"email:::"<<email<<"id:::"<<id<<"friend_id:::"<<friend_id;
                    }
                }
                //更新他的数据库,先判断好友是否为空
                QString HisNewfriend_id = "";
                if(hisfriend_id == ""){
                    HisNewfriend_id = id;
                }else{
                    HisNewfriend_id = hisfriend_id + " " + id;
                }
                query.prepare("UPDATE userInfo SET friend_id = :friend_id WHERE name = :name");
                query.bindValue(":friend_id",HisNewfriend_id);
                query.bindValue(":name",username);
                if(!query.exec())
                {
                    qDebug() << "更新失败!";
                }
                else
                {
                    qDebug() << "更新成功！";
                    query.exec("select * from userInfo");
                    while (query.next())
                    {
                        QString userName = query.value(0).toString();
                        QString passwd = query.value(1).toString();
                        QString email = query.value(2).toString();
                        QString friend_id = query.value(3).toString();
                        QString id = query.value(4).toString();
                        qDebug() << "userName:::"<< userName << "passwd:::" << passwd<<"email:::"<<email<<"id:::"<<id<<"friend_id:::"<<friend_id;
                    }
                }
            }
        }
    }
    db2.close();
}

void user_list::on_pushButton_clicked()
{
    emit showAddUser();
}

void user_list::on_pushButton_2_clicked()
{
    emit showAddGroup();
}

void user_list::on_pushButton_3_clicked()
{
    emit showJoinGroup();
}
