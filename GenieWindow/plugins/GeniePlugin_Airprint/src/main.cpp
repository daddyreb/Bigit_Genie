#include <QtGui/QApplication>
#include "airprintwidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    AirprintWidget * w = new AirprintWidget();
    w->show();
    return a.exec();
}
