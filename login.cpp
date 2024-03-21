#include "login.h"
#include "ui_login.h"
#include "passwdedit.h"
#include "register.h"
#include "systemtrayicon.h"
#include "user_list.h"
#include <QMessageBox>
#include <QUrl>
#include <QDesktopServices>
#include <QDir>
#include <QDebug>
#include <QMenu>
#include <QHostAddress>
#include <QNetworkInterface>

//窗口渐变
float opacity1 = 0.0, opacity2 = 1.0;

Login::Login(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);

    init();

    connect(this,SIGNAL(close()),this,SLOT(close()));
}

Login::~Login()
{
    delete ui;
}

void Login::init()//初始化窗口
{
    setWindowTitle(tr("登录"));//窗口名字

    m_Drag = false;//

    timer1 = new QTimer;//
    timer1->start(5);//
    timer2 = new QTimer;//
    connect(timer1, SIGNAL(timeout()), this, SLOT(slot_timer1()));
    connect(timer2, SIGNAL(timeout()), this, SLOT(slot_timer2()));

    configWindow();//UI界面设置  去边框，最小化，最大化button
    init_sql();//初始化界面密码，帐号的初值

    //init记住密码
    ui->checkBox_rPasswd->setChecked(true);
    ui->lineEdit_passwd->setEchoMode(QLineEdit::Password);
}

void Login::get_user_info()
{
    user_info_stu.userName.clear();
    user_info_stu.userName = ui->cBox_account->currentText();
    user_info_stu.passwd.clear();
    user_info_stu.passwd = ui->lineEdit_passwd->text();
}

void Login::configWindow()
{

    // 填充背景图片
    QPalette palette;
    palette.setBrush(this->backgroundRole(),QBrush(QPixmap(":/images/background.png")));
    this->setPalette(palette);

    //去掉窗口边框
    setWindowFlags(Qt::FramelessWindowHint);

    //程序init时,设置top ,user img
    set_top_img(true, -1);//设置图片显示为随机显示
    set_user_img(true, -1);//设置user图片为随机显示

    //设置UI的按钮button
    set_button();


    //设置任务栏系统托盘 start
    QStringList strList;
    strList << "Rose" << "Login";//login 为设置trayico的显示提示
    QIcon icon(":/images/ico/login_tray.png");
    SystemTrayIcon *trayIcon = new SystemTrayIcon(strList, icon, this);

    connect(trayIcon, SIGNAL(signal_lang_refresh()), this, SLOT(refresh()));//关联语言设置函数,刷新界面
    //设置任务栏系统托盘 end

}


void Login::init_sql()
{
    db1 = QSqlDatabase::addDatabase("QSQLITE","db1");
    db1.setDatabaseName("database.db");
    QSqlQuery q(db1);//需要在构造函数里指定我们上边的数据库变量。

    //获取本机ip
    QList <QHostAddress> addressList = QNetworkInterface::allAddresses();     //接收本机所有Ip地址
    QStringList addressList_str;
    for(int i = 0; i < addressList.size(); i++)
    {
        if(addressList.at(i).isNull()) continue;   //地址为空则跳过
        if(addressList.at(i).protocol() != QAbstractSocket::IPv4Protocol) continue;   //协议族不是ipv4的则跳过
        addressList_str.append(addressList.at(i).toString());      //符合条件的地址添加到列表中
    }
    QString ipv4 = addressList_str[0];//只用第一个ip即可

    if (!db1.open()){
        qDebug() << "database open fail!";
    }
    else
    {
        qDebug() << "database open success!";

        //创建一个名为userInfo的表
        QString sql_create_table = "CREATE TABLE userInfo (name VARCHAR PRIMARY KEY,passwd VARCHAR,email VARCHAR,friend_id VARCHAR,id INTEGER,ip VARCHAR)";
        q.prepare(sql_create_table);
        if(!q.exec())
        {
            qDebug()<<"creater table error";
        }

        //q.exec(QString("insert into userInfo values ('tanxin','123456','admin@qq.com','',1,'%1')").arg(ipv4));//''表示无好友，'1'表示id=1的是他好友，'1 2'表示id=1和id=2都是他好友
        q.exec(QString("insert into userInfo values ('tanxin','123456','admin@qq.com','',1,'127.0.0.1')"));// 测试使用
        q.exec("select * from userInfo");

        while (q.next())
        {
            QString userName = q.value(0).toString();
            ui->cBox_account->addItem(userName);
            QString passwd = q.value(1).toString();
            QString email = q.value(2).toString();
            QString friend_id = q.value(3).toString();
            QString id = q.value(4).toString();
            QString ip = q.value(5).toString();
            userPasswd.append(passwd);
            qDebug() << "userName:::"<< userName << "passwd:::" << passwd<<"email:::"<<email<<"friend_id:::"<<friend_id<<"id:::"<<id<<"ip:::"<<ip;
        }
        ui->cBox_account->setCurrentIndex(0);
        ui->lineEdit_passwd->setText(userPasswd.at(0));
    }
    db1.close();
    qDebug()<<"database closed!";
}

void Login::set_top_img(bool isSandom, int index_img)
{
    //427 185
    int set_index_img = 1;
    if(isSandom == true)//随机显示topimg
    {

        QTime time_sand;
        time_sand= QTime::currentTime();//获取当前时间
        qsrand(time_sand.msec()+time_sand.second()*1000);
        set_index_img = qrand()%6 + 1 ;//在1-6中选出随机数

    }
    if(isSandom == false) //不随机显示，按index_img显示图片s
    {
        set_index_img = index_img;
    }

    QString top_img_path=":/images/top_img1.jpg";
    QImage top_img;
    top_img_path = ":/images/top_img" + QString::number(set_index_img, 10)  + ".jpg";
    qDebug()<< "[leo]" << top_img_path;
    top_img.load(top_img_path);
    QPixmap top_pic=QPixmap::fromImage(top_img.scaled(ui->label_top_img->width(),ui->label_top_img->height()));
    ui->label_top_img->setPixmap(top_pic);
}

void Login::set_button()
{
    //构建最小化、关闭按钮,设置按钮
    minBtn = new QToolButton(this);
    closeBbtn = new QToolButton(this);
    setBtn = new QToolButton(this);

    //获取界面的宽度
    int width = this->width();
    //设置最小化、关闭按钮在界面的位置
    minBtn->setGeometry(width-55,5,20,20);
    closeBbtn->setGeometry(width-25,5,20,20);
    setBtn->setGeometry(width-80,7,15,15);

    //设置鼠标移至按钮上的提示信息
    minBtn->setToolTip(tr("最小化"));
    closeBbtn->setToolTip(tr("关闭"));
    setBtn->setToolTip(tr("设置"));

    //设置最小化、关闭按钮的样式图标
    minBtn->setIcon(QIcon(":/images/ico/mini.png"));
    minBtn->setStyleSheet("background-color:transparent;");
    closeBbtn->setIcon(QIcon(":/images/ico/close.png"));
    closeBbtn->setStyleSheet("background-color:transparent;");
    setBtn->setIcon(QIcon(":/images/ico/setting.png"));
    setBtn->setStyleSheet("background-color:transparent;");

    //关联最小化、关闭按钮的槽函数
    connect(minBtn, SIGNAL(clicked()), this, SLOT(slot_minWindow()));
    connect(closeBbtn, SIGNAL(clicked()), this, SLOT(slot_closeWindow()));
    connect(setBtn, SIGNAL(clicked()), this, SLOT(slot_setLanguage()));

    create_menuLanguage();//创建语言菜单
}

void Login::set_user_img(bool isSandom, int index_img)
{
    //40,182 85 85
    int set_index_img = 1;
    if(isSandom == true)//随机显示userimg
    {
        QTime time_sand;
        time_sand= QTime::currentTime();//获取当前时间
        qsrand(time_sand.msec()+time_sand.second()*1000);
        set_index_img = qrand()%8;//在0-8中选出随机数

    }
    if(isSandom == false) //不随机显示，按index_img显示图片s
    {
        set_index_img = index_img;
    }

    QString user_img_path=":/images/ico/user1.jpeg";
    QImage user_img;
    user_img_path = ":/images/ico/user" + QString::number(set_index_img, 10)  + ".jpeg";
    qDebug()<< "[leo]user" << user_img_path;
    user_img.load(user_img_path);
    QPixmap img_pic=QPixmap::fromImage(user_img.scaled(ui->label_user_img->width(),
                                                       ui->label_user_img->height()));
    ui->label_user_img->setPixmap(img_pic);
}

void Login::create_menuLanguage()
{
    //语言菜单
    act_chinese = new QAction(tr("简体中文"), this);
    act_english = new QAction(tr("English"), this);
    menu1 = new QMenu;
    menu1->addAction(act_chinese);
    menu1->addAction(act_english);
    connect(menu1, SIGNAL(triggered(QAction*)), this, SLOT(slot_actGrp(QAction*)));
}

void Login::setStyle(const QString &style)
{
    QFile qss( ":/images/qss/" + style);//black.qss
    qDebug() << "qss                 :";
    qss.open(QIODevice::ReadOnly | QIODevice::Text);
    qApp->setStyleSheet(qss.readAll());
    qss.close();
}

//解决无边框后产生的问题
void Login::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {
        m_Drag = true;
        m_point = e->globalPos() - this->pos();
        e->accept();
        //  qDebug()<<"leo";
    }
}
void Login::mouseMoveEvent(QMouseEvent *e)
{
    if (m_Drag && (e->buttons() & Qt::LeftButton)) {
        move(e->globalPos() - m_point);
        e->accept();
        // qDebug()<<"leomove";
    }
}
void Login::mouseReleaseEvent(QMouseEvent *e)
{
    m_Drag = false;
}

void Login::on_btn_login_clicked()
{
    qDebug() << "login:" << user_info_stu.userName << user_info_stu.passwd;
    if(ui->cBox_account->currentText().isEmpty() ||
            ui->lineEdit_passwd->text().isEmpty()){
        QMessageBox::warning(this,tr("警告"),tr("请输入用户名和密码！"));
    }
    else
    {
        int is_use_exist_flag = 0;       //判断用户是否存在
        int is_use_nampwd_check_flag = 0;       //判断用户名和密码是否匹配
        get_user_info();

        if(!db1.open())
        {
            qDebug() << "database open fail login!";
        }
        else
        {
            QSqlQuery query(db1);
            qDebug() << "database open success login!";
            query.exec("select * from userInfo");
            while (query.next())
            {
                QString userName = query.value(0).toString();
                QString passwd = query.value(1).toString();
                qDebug() << "login userName:::"<< userName << "passwd:::" << passwd;

                if(userName == user_info_stu.userName){
                    is_use_exist_flag = true;              //用户存在
                    if(passwd == user_info_stu.passwd){
                        is_use_nampwd_check_flag = true;//用户名和密码匹配
                        emit close();
                        emit showWidget(userName);
                    }
                }
            }

            if(is_use_exist_flag == false)
            {
                QMessageBox::information(this,tr("提示"),tr("用户不存在！"));
            }
            else
            {
                if(is_use_nampwd_check_flag == false)
                {
                    QMessageBox::warning(this,tr("警告"),tr("用户密码错误！"));
                }
            }
        }

        db1.close();
    }
}

void Login::recvW()
{
    this->show();
}

//注册button
void Login::on_btn_regist_clicked()
{
    Register r;
    r.setParent(this);//设置父对象
    r.exec();//注册页面r，仅仅获取信息.

    //获取本机ip
    QList <QHostAddress> addressList = QNetworkInterface::allAddresses();//接收本机所有Ip地址
    QStringList addressList_str;
    for(int i = 0; i < addressList.size(); i++)
    {
        if(addressList.at(i).isNull()) continue;//地址为空则跳过
        if(addressList.at(i).protocol() != QAbstractSocket::IPv4Protocol) continue;//协议族不是ipv4的则跳过
        addressList_str.append(addressList.at(i).toString());//符合条件的地址添加到列表中
    }
    QString ipv4 = addressList_str[0];//只用第一个ip即可

    if(user_info_stu.userName.isEmpty() || user_info_stu.passwd.isEmpty()){
        QMessageBox::information(this,tr("提示"),tr("请输入用户名和密码！"));
    }
    else
    {
        bool exitFlag = false;//判断用户是否存在

        if(!db1.open())
        {
            qDebug() << "database open fail regist!";
        }
        else
        {
            int my_id = 0;
            QSqlQuery query(db1);
            qDebug() << "database open success regist!";
            query.exec("select * from userInfo");
            while (query.next())
            {
                QString userName = query.value(0).toString();
                QString passwd = query.value(1).toString();
                my_id = query.value(4).toInt();
                my_id += 1;
                if(userName == user_info_stu.userName){
                    exitFlag = true;//用户存在
                }
            }

            if(exitFlag == false){
                query.exec(QString("insert into userInfo values ('%1','%2','%3','%4',%5,'%6')")
                           .arg(user_info_stu.userName).arg(user_info_stu.passwd)
                           .arg(user_info_stu.email).arg("").arg(my_id).arg(ipv4));

                ui->cBox_account->addItem(user_info_stu.userName);
                userPasswd.append(user_info_stu.passwd);
                QMessageBox::information(this,tr("提示"),tr("注册成功！"));

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
            }else{
                QMessageBox::warning(this,tr("警告"),tr("用户已存在！"));
            }
        }
        db1.close();
    }
}

//修改密码button
void Login::on_btn_edit_pwd_clicked()
{
    if(ui->cBox_account->currentText().isEmpty() ||
            ui->lineEdit_passwd->text().isEmpty()){
        QMessageBox::information(this,tr("提示"),tr("请输入用户名和密码！"));
    }
    else
    {
        bool is_use_exist_flag = false;//判断用户是否存在
        bool is_use_nampwd_check_flag = false;//判断用户名和密码是否匹配
        get_user_info();

        if(!db1.open())
        {
            qDebug() << "database open fail login!";
        }
        else
        {
            QSqlQuery query(db1);
            qDebug() << "database open success login!";
            query.exec("select * from userInfo");
            while (query.next())
            {
                QString userName = query.value(0).toString();
                QString passwd = query.value(1).toString();
                qDebug() << "edit userName:::"<< userName << "passwd:::" << passwd;

                if(userName == user_info_stu.userName)
                {
                    is_use_exist_flag = true;//用户存在
                    if(passwd == user_info_stu.passwd)
                    {
                        is_use_nampwd_check_flag = true;//用户名和密码匹配
                        passwdEdit passwd_edit;
                        passwd_edit.setLogin(this);//设置父对象
                        passwd_edit.exec();//修改页面passwd，仅仅获取信息.
                        if(passwd != user_info_stu.passwd){
                            query.prepare("UPDATE userInfo SET passwd = :passwd WHERE name = :name");
                            query.bindValue(":passwd",user_info_stu.passwd);
                            query.bindValue(":name",user_info_stu.userName);
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
                            QMessageBox::information(this,tr("提示"),tr("密码修改成功！"));
                        }
                    }
                }
            }

            if(is_use_exist_flag == false)
            {
                QMessageBox::information(this,tr("提示"),tr("用户不存在！"));
            }
            else
            {
                if(is_use_nampwd_check_flag == 0){
                    QMessageBox::warning(this,tr("警告"),tr("用户密码错误！"));
                }
            }
        }
        db1.close();
    }
}

//最小化button
void Login::slot_minWindow()
{
    this->showMinimized();
}

void Login::slot_closeWindow()
{
    timer2->start(5);
}

void Login::slot_setLanguage()
{
    menu1->exec(QCursor::pos());
}

void Login::slot_actGrp(QAction *act)
{
    if(act == act_chinese)
    {
        qDebug() << "china ";
        translator.load(":/cn.qm");
        qApp->installTranslator(&translator);
    }
    if(act == act_english)
    {
        qDebug() << "english ";
        translator.load(":/en.qm");
        qApp->installTranslator(&translator);
    }
    this->refresh();//刷新托盘相关文字
    emit signal_lang_refresh();//发送刷新页面文字的信号
}

void Login::slot_timer1()
{
    if (opacity1 >= 1.0) {
        timer1->stop();
    }else{
        opacity1 += 0.01;
    }
    setWindowOpacity(opacity1);//设置窗口透明度
}

void Login::slot_timer2()
{
    if (opacity2 <= 0.0) {
        timer2->stop();

        this->close();
    }else{
        opacity2 -= 0.01;
    }
    setWindowOpacity(opacity2);//设置窗口透明度
}

void Login::on_cBox_account_activated(int index)
{
    ui->lineEdit_passwd->setText(userPasswd.at(index));
    qDebug() << "change cBox:" << ui->cBox_account->currentText()
             << userPasswd.at(index);
}


//下拉框选里面的项时，会切换top_img的图片和头像图片
void Login::on_cBox_account_currentIndexChanged(int index)
{
    set_top_img(true,index);
    set_user_img(true,index);
}

void Login::refresh()//刷新login页面当前的字符串。其它页面无需刷新，因为打开时候，会自动刷新相关字符串。而主窗口不会。
{
    ui->btn_login->setText(tr("登录"));
    ui->btn_edit_pwd->setText(tr("找回密码"));
    ui->btn_regist->setText(tr("注册用户"));
    ui->label->setText(tr("记住密码"));
    ui->label_2->setText(tr("自动登录"));
}

