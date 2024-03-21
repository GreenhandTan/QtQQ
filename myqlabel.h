#ifndef MYQLABEL_H
#define MYQLABEL_H

#include <QWidget>
#include <QTime>

namespace Ui {
class myQLabel;
}

class myQLabel : public QWidget
{
    Q_OBJECT

public:
    explicit myQLabel(QWidget *parent = nullptr,QString username = "");
    ~myQLabel();

private:
    Ui::myQLabel *ui;
    QTime time_sand;
};

#endif // MYQLABEL_H
