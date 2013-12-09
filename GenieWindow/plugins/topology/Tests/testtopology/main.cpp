#include <QtCore>
#include <QtGui>
#include "TestTopologyForm.h"

int main(int argc,char **argv)
{
#ifdef Q_OS_MACX
    signal(SIGPIPE,SIG_IGN);
#endif
    QApplication app(argc,argv);
    TestTopologyForm w;
    w.show();
    return app.exec();
}

