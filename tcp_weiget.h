#ifndef TCP_WEIGET_H
#define TCP_WEIGET_H

#include <QWidget>
#include <QTextCharFormat>
#include <QColorDialog>
#include <QMessageBox>
#include <QFileDialog>
#include <QTextStream>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QTcpSocket>
#include <QDateTime>
#include <QMediaPlayer>
#include <QSound>
#include <QTcpServer>

namespace Ui {
class Tcp_Weiget;
}

class Tcp_Weiget : public QWidget
{
    Q_OBJECT

public:
    explicit Tcp_Weiget(QWidget *parent,QString chatName,QString myName);
    ~Tcp_Weiget();
    void userInit();
    QMediaPlayer *startSound;  //创建一个音乐播放器
    QSound *messSound;

protected:
    bool saveFile(const QString &filename);
    void closeEvent(QCloseEvent *event);

private:
    Ui::Tcp_Weiget *ui;
    qint16 port;
    QColor color;
    QString myname;
    QString chatname;
    QString ipv4;//聊天对象ip
    QString Host_ipv4;//自己的ip
    int chat_port = 8888;
    bool severe_stats;//服务端开启状态
    QSqlDatabase db3;
    bool connectState;//客户端连接状态
    QTcpServer *severe;//服务端
    QTcpSocket *socket;
    QTcpSocket *clintSock;
    QList <QTcpSocket*> clintList_sock;   //多个客户端的套接字列表

private slots:
    void on_fontCbx_currentFontChanged(const QFont &f);
    void on_sizeCbx_currentIndexChanged(const QString &arg1);
    void on_boldTBtn_clicked(bool checked);
    void on_italicTBtn_clicked(bool checked);
    void on_underlineTBtn_clicked(bool checked);
    void on_colorTBtn_clicked();
    void curFmtChanged(const QTextCharFormat &fmt);
    void on_clearTBtn_clicked();
    void on_exitBtn_clicked();
    void on_saveTBtn_clicked();

    void readMessage_clint();//作为客户端接收信息
    void readMessage_serve();//作为服务端接收信息
    void newconnectSlot();//新的连接加入
    void disconnectSlot();//断开连接槽函数
    void serveStartInit();//服务器开始监听
    void clintStartInit();//客户端开始连接

    void on_sendBtn_clicked();
};

#endif // TCP_WEIGET_H
