#include "tcp_weiget.h"
#include "ui_tcp_weiget.h"

Tcp_Weiget::Tcp_Weiget(QWidget *parent,QString chatName,QString myName) :
    QWidget(parent),
    ui(new Ui::Tcp_Weiget)
{
    ui->setupUi(this);

    //初始化数据库连接
    db3 = QSqlDatabase::addDatabase("QSQLITE","db3");
    db3.setDatabaseName("database.db");

    myname = myName;
    chatname = chatName;//初始化聊天对象
    userInit();//初始化聊天对象ipv4
    qDebug()<<"初始化聊天对象："<<chatName<<"他的ip："<<ipv4<<"当前聊天端口："<<chat_port;
    messSound = new QSound(":/sounds/message.wav", this);

    severe_stats = false;
    socket = new QTcpSocket(this);//先创建客户端
    connectState = false;//未连接状态
    clintStartInit();
}

Tcp_Weiget::~Tcp_Weiget()
{
    delete ui;
}
//服务端监听初始化函数
void Tcp_Weiget::serveStartInit()
{
    QHostAddress currentHostIP = QHostAddress(Host_ipv4);//地址字符串转换成主机Ip127.0.0.1
    severe->listen(currentHostIP,chat_port);
    severe_stats = true;
    connect(severe, &QTcpServer::newConnection, this, &Tcp_Weiget::newconnectSlot);//有新连接时传入信号
}
void Tcp_Weiget::clintStartInit()
{
    if(!connectState)//客户端还未连接服务端
    {
        socket->connectToHost(ipv4, chat_port);//连接服务端
        if(socket->waitForConnected())//等待连接成功
        {
            ui->msgBrowser->append(tr("%1已连接！").arg(chatname));
            connectState = true;
        }else{
            //连接失败，对方未开启
            socket->close();//关闭客户端
            severe = new QTcpServer(this);//创建服务端
            serveStartInit();
        }
    }else{
        //客户端已经连接
        socket->close();//关闭套接字，此时会发送disconnected信号
    }
    connect(socket, &QTcpSocket::readyRead, this, &Tcp_Weiget::readMessage_clint);//接收信息
    connect(socket, &QTcpSocket::disconnected, this, &Tcp_Weiget::disconnectSlot);//打印断开连接信息
}

//新的客户端连接
void Tcp_Weiget::newconnectSlot()
{
    clintSock = severe->nextPendingConnection();//获取当前连接的客户端套接字
    clintList_sock.push_back(clintSock);//将连接的客户端放入客户端列表中
    QString str = chatname + "已连接!";
    ui->msgBrowser->append(str);
    connect(clintSock, &QTcpSocket::readyRead, this, &Tcp_Weiget::readMessage_serve);  //准备接受信息
    connect(clintSock, &QTcpSocket::disconnected, this, &Tcp_Weiget::disconnectSlot);   //客户端的断开连接信息
}
//打印连接断开信息
void Tcp_Weiget::disconnectSlot()
{
    ui->msgBrowser->append(tr("%1已离开！").arg(chatname));
}

//发送信息
void Tcp_Weiget::on_sendBtn_clicked()
{
    QString str = ui->msgTxtEdit->toPlainText();
    if(severe_stats){
        //服务端发送信息
        for(int i = 0; i < clintList_sock.count(); i++){
            clintList_sock.at(i)->write(str.toUtf8());
        }
    }else{
        //客户端发送信息
         if(socket->isOpen() && socket->isValid())
         {
             socket->write(str.toUtf8());//给服务端发送信息
         }
    }
    ui->msgTxtEdit->clear();
    QString showStr = QDateTime::currentDateTime().toString("dddd yyyy.MM.dd hh:mm:ss") + '\n' + str;
    ui->msgBrowser->append(showStr);//显示自己发送的信息
}

//客户端接收信息
void Tcp_Weiget::readMessage_clint()
{
    messSound->play();
    QByteArray arr = socket->readAll();
    QString str;
    str = QDateTime::currentDateTime().toString("dddd yyyy.MM.dd hh:mm:ss") + '\n' + arr.data();
    ui->msgBrowser->append(str);//显示信息
}
//服务端接收信息及给其他客户端发送信息
void Tcp_Weiget::readMessage_serve()
{
    QTcpSocket*  currentClint;
    QByteArray arr;
    QString str;
    if(!clintList_sock.isEmpty())//有客户端存在
    {
        for(int i = 0; i < clintList_sock.count(); i++)     //服务端接收信息
        {
            arr = clintList_sock.at(i)->readAll();      //接收客户端发送的字节信息
            if(arr.isNull())  continue;   //空代表不是该客户端发送
            messSound->play();
            currentClint = clintList_sock.at(i);
            str = QDateTime::currentDateTime().toString("dddd.yyyy.MM.dd HH:mm:ss") + '\n' + arr.data();
            break;
        }
        ui->msgBrowser->append(str);//显示信息
        for(int i = 0; i < clintList_sock.count(); i++)     //给其它客户端发送信息
        {
            QTcpSocket *temp = clintList_sock.at(i);
            if(currentClint == temp)  continue;      //遇到自己就跳过
            temp->write(str.toUtf8());   //发送信息
        }
    }
}

//获取聊天对象ip
void Tcp_Weiget::userInit()
{
    QSqlQuery query(db3);
    if(!db3.open())
    {
        qDebug() << "database open fail regist!";
    }else{
        query.exec(QString("select * from userInfo where name = '%1'").arg(chatname));
        while (query.next())
        {
            ipv4 = query.value(5).toString();
        }
        query.exec(QString("select * from userInfo where name = '%1'").arg(myname));
        while (query.next())
        {
            Host_ipv4 = query.value(5).toString();
        }
    }
    db3.close();
}

/*以下函数不可动
 * 为样式控制函数
 * */
bool Tcp_Weiget::saveFile(const QString &filename){
    QFile file(filename);
    if(!file.open(QFile::WriteOnly|QFile::Text)){
        QMessageBox::warning(this,tr("保存文件"),tr("无法保存文件%1:\n%2").arg(filename).arg(file.errorString()));

        return false;

    }
    QTextStream out(&file);
    out<<ui->msgBrowser->toPlainText();
    return true;

}
void Tcp_Weiget::on_fontCbx_currentFontChanged(const QFont &f)
{
    ui->msgTxtEdit->setCurrentFont(f);
    ui->msgTxtEdit->setFocus();
}
//字体大小
void Tcp_Weiget::on_sizeCbx_currentIndexChanged(const QString &arg1)
{
    ui->msgTxtEdit->setFontPointSize(arg1.toDouble());
    ui->msgTxtEdit->setFocus();
}
//字体
void Tcp_Weiget::on_boldTBtn_clicked(bool checked)
{
    if(checked)ui->msgTxtEdit->setFontWeight(QFont::Bold);
    else ui->msgTxtEdit->setFontWeight(QFont::Normal);
    ui->msgTxtEdit->setFocus();
}
//
void Tcp_Weiget::on_italicTBtn_clicked(bool checked)
{
    ui->msgTxtEdit->setFontItalic(checked);
    ui->msgTxtEdit->setFocus();
}
//下划线
void Tcp_Weiget::on_underlineTBtn_clicked(bool checked)
{
    ui->msgTxtEdit->setFontUnderline(checked);
    ui->msgTxtEdit->setFocus();
}
//字体颜色
void Tcp_Weiget::on_colorTBtn_clicked()
{
    color=QColorDialog::getColor(color,this);
    if(color.isValid()){
        ui->msgTxtEdit->setTextColor(color);
        ui->msgTxtEdit->setFocus();
    }
}
void Tcp_Weiget::curFmtChanged(const QTextCharFormat &fmt){
    ui->fontCbx->setCurrentFont(fmt.font());
    if(fmt.fontPointSize()<8){
        ui->sizeCbx->setCurrentIndex(4);

    }else{
        ui->sizeCbx->setCurrentIndex(ui->sizeCbx->findText(QString::number(fmt.fontPointSize())));
    }
    ui->boldTBtn->setChecked(fmt.font().bold());
    ui->italicTBtn->setChecked(fmt.font().italic());
    ui->underlineTBtn->setChecked(fmt.font().underline());
    color=fmt.foreground().color();

}
//保存聊天记录
void Tcp_Weiget::on_saveTBtn_clicked()
{
    if (ui->msgBrowser->document()->isEmpty()){
        QMessageBox::warning(0,tr("警告"),tr("聊天记录为空，无法保存！"),QMessageBox::Ok);
    }else {
        QString fname=QFileDialog::getSaveFileName(this,tr("保存聊天记录"),tr("聊天记录"),tr("文本(*.txt);;所有文件(*.*)"));
        if(!fname.isEmpty())saveFile(fname);
    }
}
void Tcp_Weiget::closeEvent(QCloseEvent *event){
    QWidget::closeEvent(event);
}
//清空页面按钮
void Tcp_Weiget::on_clearTBtn_clicked()
{
    ui->msgBrowser->clear();
}
//退出键
void Tcp_Weiget::on_exitBtn_clicked()
{
    close();
}
