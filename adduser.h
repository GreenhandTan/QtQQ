#ifndef ADDUSER_H
#define ADDUSER_H

#include <QWidget>

namespace Ui {
class AddUser;
}

class AddUser : public QWidget
{
    Q_OBJECT

public:
    explicit AddUser(QWidget *parent = nullptr);
    ~AddUser();

signals:
    void addUser();
    void addUser(QString username);

public slots:
    void recvAddUser();
    void closeAddUser();

private slots:
    void on_pushButton_clicked();

private:
    Ui::AddUser *ui;
};

#endif // ADDUSER_H
