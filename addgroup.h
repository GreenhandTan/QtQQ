#ifndef ADDGROUP_H
#define ADDGROUP_H

#include <QWidget>
#include "udp_weiget.h"
#include "user_list.h"

namespace Ui {
class AddGroup;
}

class AddGroup : public QWidget
{
    Q_OBJECT

public:
    explicit AddGroup(QWidget *parent = nullptr);
    ~AddGroup();

signals:
    void signal_lang_refresh();

public slots:
    void recvAddGroup();
    void closeAddGroup();

private slots:
    void on_pushButton_clicked();

private:
    Ui::AddGroup *ui;
    Udp_Weiget *groupWidget;//对应聊天窗口对象的指针

};

#endif // ADDGROUP_H
