#include <QApplication>
#include <QWidget>
#include "QMyWidget.h"

int main(int argc,char **argv)
{
    QApplication a(argc,argv);
    QWidget *w=new QMyWidget;
    w->show();
    return a.exec();
}
