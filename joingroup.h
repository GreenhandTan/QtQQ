#ifndef JOINGROUP_H
#define JOINGROUP_H

#include <QWidget>
#include "udp_weiget.h"
#include "user_list.h"

namespace Ui {
class JoinGroup;
}

class JoinGroup : public QWidget
{
    Q_OBJECT

public:
    explicit JoinGroup(QWidget *parent = nullptr);
    ~JoinGroup();

public slots:
    void recvJoinGroup();
    void closeJoinGroup();

private slots:
    void on_pushButton_clicked();

    void on_checkBox_stateChanged(int arg1);

private:
    Ui::JoinGroup *ui;
    Udp_Weiget *groupWidget;//对应聊天窗口对象的指针
};

#endif // JOINGROUP_H
