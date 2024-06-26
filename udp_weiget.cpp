#include "udp_weiget.h"
#include "ui_udp_weiget.h"

Udp_Weiget::Udp_Weiget(QWidget *parent,QString usrname) :
    QWidget(parent),
    ui(new Ui::Udp_Weiget)
{
    ui->setupUi(this);
    ui->sendBtn->setShortcut(tr("Ctrl+S"));
    uName=usrname;
    udpSocket=new QUdpSocket(this);
    port=23232;
    udpSocket->bind(port,QUdpSocket::ShareAddress|QUdpSocket::ReuseAddressHint);
    connect(udpSocket,SIGNAL(readyRead()),this,SLOT(processPendingDatagrams()));
    sndMsg(UsrEnter);
    srv=new Server(this);
    connect(srv,SIGNAL(sndFileName(QString)),this,SLOT(getFileName(QString)));
    connect(ui->msgTxtEdit,SIGNAL(currentCharFormatChanged(QTextCharFormat)),this,SLOT(curFmtChanged(QTextCharFormat)));

    messSound = new QSound(":/sounds/message.wav", this);
}

Udp_Weiget::~Udp_Weiget()
{
    delete ui;
}

void Udp_Weiget::sndMsg(MsgType type,QString srvaddr)
{
    QByteArray data;
    QDataStream out(&data,QIODevice::WriteOnly);
    QString address=getIP();
    out<<type<<getUsr();
    switch (type) {
    case Msg:
        if(ui->msgTxtEdit->toPlainText()=="")
        {
            QMessageBox::warning(0,tr("Warning"),("发送的信息不能为空！"),QMessageBox::Ok);
            return;
        }
        out<<address<<getMsg();
        ui->msgBrowser->verticalScrollBar()->setValue(ui->msgBrowser->verticalScrollBar()->maximum());
        break;
    case UsrEnter:
        out<<address;
        break;
    case UsrLeft:
        break;
    case FileName:
    {
        int row=ui->usrTblWidget->currentRow();
        QString clntaddr=ui->usrTblWidget->item(row,1)->text();
        out<<address<<clntaddr<<fileName;
        break;
    }
    case Refuse:
    {
        out<<srvaddr;
        break;
    }
    }
    udpSocket->writeDatagram(data,data.length(),QHostAddress::Broadcast,port);

}

void Udp_Weiget::processPendingDatagrams(){
    while(udpSocket->hasPendingDatagrams()){

        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());
        udpSocket->readDatagram(datagram.data(),datagram.size());
        QDataStream in(&datagram,QIODevice::ReadOnly);
        int MsgType;
        in>>MsgType;
        QString usrName,ipAddr,msg;
        QString time=QDateTime::currentDateTime().toString("yyyy-MM-ddhh:mm:ss");
        switch (MsgType) {
        case Msg:in>>usrName>>ipAddr>>msg;
            ui->msgBrowser->setTextColor(Qt::blue);
            ui->msgBrowser->setCurrentFont(QFont("Times New Roman",12));
            ui->msgBrowser->append("["+usrName+"]"+time);
            ui->msgBrowser->append(msg);
            break;
        case UsrEnter:
            in>>usrName>>ipAddr;
            usrEnter(usrName,ipAddr);
            break;
        case UsrLeft:
            in>>usrName;
            usrLeft(usrName,time);
            break;
        case FileName:{
            in>>usrName>>ipAddr;
            QString clntAddr,fileName;
            in>>clntAddr>>fileName;
            hasPendingFile(usrName,ipAddr,clntAddr,fileName);
            break;}

        case Refuse:
            in>>usrName;
            QString srvAddr;
            in>>srvAddr;
            QString ipAddr=getIP();
            if(ipAddr==srvAddr){
                srv->refused();
            }
            break;

        }

    }

}

void Udp_Weiget::usrEnter(QString usrname, QString ipaddr){
    bool isEmpty=ui->usrTblWidget->findItems(usrname,Qt::MatchExactly).isEmpty();
    if(isEmpty){
        QTableWidgetItem *usr=new QTableWidgetItem(usrname);
        QTableWidgetItem *ip=new QTableWidgetItem(ipaddr);
        ui->usrTblWidget->insertRow(0);
        ui->usrTblWidget->setItem(0,0,usr);
        ui->usrTblWidget->setItem(0,1,ip);
        ui->msgBrowser->setTextColor(Qt::gray);
        ui->msgBrowser->setCurrentFont(QFont("Times New Roman",10));
        ui->msgBrowser->append(tr("%1在线！").arg(usrname));
        ui->usrNumLbl->setText(tr("在线人数：%1").arg(ui->usrTblWidget->rowCount()));
        sndMsg(UsrEnter);
    }
}

void Udp_Weiget::usrLeft(QString usrname, QString time){
    int rowNum=ui->usrTblWidget->findItems(usrname,Qt::MatchExactly).first()->row();
    ui->usrTblWidget->removeRow(rowNum);
    ui->msgBrowser->setTextColor(Qt::gray);
    ui->msgBrowser->setCurrentFont(QFont("Times New Roman",10));
    ui->msgBrowser->append(tr("%1于%2离开").arg(usrname).arg(time));
    ui->usrNumLbl->setText(tr("在线人数：%1").arg(ui->usrTblWidget->rowCount()));
}

QString Udp_Weiget::getIP(){
    QList<QHostAddress>list=QNetworkInterface::allAddresses();
    foreach (QHostAddress addr, list) {
        if(addr.protocol()==QAbstractSocket::IPv4Protocol)
            return addr.toString();
    }
    return 0;
}

QString Udp_Weiget::getUsr(){
    return uName;
}

QString Udp_Weiget::getMsg(){
    QString msg=ui->msgTxtEdit->toHtml();
    ui->msgTxtEdit->clear();
    ui->msgTxtEdit->setFocus();
    return msg;
}

void Udp_Weiget::on_sendBtn_clicked(){
    messSound->play();
    sndMsg(Msg);
}

void Udp_Weiget::getFileName(QString name){

    fileName=name;
    sndMsg(FileName);

}

void Udp_Weiget::on_sendTBtn_clicked(){
    if(ui->usrTblWidget->selectedItems().isEmpty())
    {
        QMessageBox::warning(0,tr("选择用户"),tr("请先选择目标用户！！"),QMessageBox::Ok);
        return;
    }
    srv->show();
    srv->initSrv();

}

void Udp_Weiget::hasPendingFile(QString usrname, QString srvaddr, QString clntaddr, QString filename){
    QString ipAddr=getIP();
    if(ipAddr==clntaddr)
    {
        int btn=QMessageBox::information(this,tr("接收文件"),tr("来自%1(%2)的文件：%3，是否接收？").arg(usrname).arg(srvaddr).arg(filename),QMessageBox::Yes,QMessageBox::No);
        if(btn==QMessageBox::Yes){
            QString name=QFileDialog::getSaveFileName(0,tr("保存文件"),filename);
            if(!name.isEmpty()){

                Client *clnt=new Client(this);
                clnt->setFileName(name);
                clnt->setHostAddr(QHostAddress(srvaddr));
                clnt->show();
            }
        }else{
            sndMsg(Refuse,srvaddr);
        }
    }

}

void Udp_Weiget::on_fontCbx_currentFontChanged(const QFont &f){
    ui->msgTxtEdit->setCurrentFont(f);
    ui->msgTxtEdit->setFocus();
}

void Udp_Weiget::on_sizeCbx_currentIndexChanged(const QString &arg1){
    ui->msgTxtEdit->setFontPointSize(arg1.toDouble());
    ui->msgTxtEdit->setFocus();
}

void Udp_Weiget::on_boldTBtn_clicked(bool checked){
    if(checked)ui->msgTxtEdit->setFontWeight(QFont::Bold);
    else ui->msgTxtEdit->setFontWeight(QFont::Normal);
    ui->msgTxtEdit->setFocus();
}

void Udp_Weiget::on_italicTBtn_clicked(bool checked){
    ui->msgTxtEdit->setFontItalic(checked);
    ui->msgTxtEdit->setFocus();
}

void Udp_Weiget::on_underlineTBtn_clicked(bool checked){
    ui->msgTxtEdit->setFontUnderline(checked);
    ui->msgTxtEdit->setFocus();
}

void Udp_Weiget::on_colorTBtn_clicked(){
    color=QColorDialog::getColor(color,this);
    if(color.isValid()){
        ui->msgTxtEdit->setTextColor(color);
        ui->msgTxtEdit->setFocus();
    }
}

void Udp_Weiget::curFmtChanged(const QTextCharFormat &fmt){
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

void Udp_Weiget::on_saveTBtn_clicked(){
    if (ui->msgBrowser->document()->isEmpty()){
        QMessageBox::warning(0,tr("警告"),tr("聊天记录为空，无法保存！"),QMessageBox::Ok);
    }else {
        QString fname=QFileDialog::getSaveFileName(this,tr("保存聊天记录"),tr("聊天记录"),tr("文本(*.txt);;所有文件(*.*)"));
        if(!fname.isEmpty())saveFile(fname);
    }
}

bool Udp_Weiget::saveFile(const QString &filename){
    QFile file(filename);
    if(!file.open(QFile::WriteOnly|QFile::Text)){
        QMessageBox::warning(this,tr("保存文件"),tr("无法保存文件%1:\n%2").arg(filename).arg(file.errorString()));
        return false;
    }
    QTextStream out(&file);
    out<<ui->msgBrowser->toPlainText();
    return true;
}

void Udp_Weiget::recvGroupWidget()
{
    this->show();
}

void Udp_Weiget::closeEvent(QCloseEvent *event){
    sndMsg(UsrLeft);
    QWidget::closeEvent(event);
}

void Udp_Weiget::on_clearTBtn_clicked()
{
    ui->msgBrowser->clear();
}

void Udp_Weiget::on_exitBtn_clicked()
{
    close();
}
