#include <QApplication>

#include "QLauncher.h"

#include <iostream>
#include <QtCore>
#include <QtGui>
#include <QDesktopServices>
//#include <crtdbg.h>
//#include <vld.h>



#ifdef Q_OS_WIN32
#include <windows.h>
#else
#include "genie2_settings.h"
#include "interface2_settings.h"
#include "QCoaCoaApplication.h"
#endif
#include "QGenieConfigFileMover.h"
//#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    switch (type) {
    case QtDebugMsg:
        fprintf(stderr, "Debug: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtWarningMsg:
        fprintf(stderr, "Warning: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtCriticalMsg:
        fprintf(stderr, "Critical: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtFatalMsg:
        fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        abort();
    }
}

int main(int argc, char *argv[])
{
    //_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG)|_CRTDBG_LEAK_CHECK_DF);
    //	int *itemp=new int;
#ifdef Q_OS_WIN32
    QApplication a(argc, argv);
#else
//    setenv("QT_NO_FAST_SCROLL",getenv("USER"),1);
    QCoaCoaApplication a(argc,argv);
#endif
    //Debug output
   // if(!a.arguments().contains("-dbg"))
//    {
//        qInstallMessageHandler(myMessageOutput);
//    }

    QGenieConfigFileMover mover;
//    QPixmapCache::setCacheLimit(1024);
#ifdef Q_OS_MACX
//    if(QApplication::arguments().contains("-firststart"))
//    {
//        QMessageBox::warning(NULL,"",CHANGABLE_INI_FNAME);
//        if(QFile::exists(CHANGABLE_INI_FNAME))
//        {
//            QFile::remove(CHANGABLE_INI_FNAME);
//        }

//        return 0;
//    }
#endif
#ifdef Q_OS_WIN32
    SetCurrentDirectoryA(qApp->applicationDirPath().toLocal8Bit());
#endif

#ifdef Q_OS_WIN32

    DWORD lockTime = 0;
    SystemParametersInfo(SPI_GETFOREGROUNDLOCKTIMEOUT, 0, &lockTime, 0);
    SystemParametersInfo(SPI_SETFOREGROUNDLOCKTIMEOUT, 0, 0,
                         SPIF_SENDWININICHANGE | SPIF_UPDATEINIFILE);

#endif

#ifdef Q_OS_MACX
    qDebug()<<qApp->libraryPaths();
    QDir dir(QApplication::applicationDirPath());
    dir.cdUp();
    dir.cd("PlugIns");
    qApp->addLibraryPath(dir.absolutePath());
    //    QApplication::setLibraryPaths(QStringList(dir.absolutePath()));
#endif
    // a.thread()->setPriority(QThread::TimeCriticalPriority);
    a.setQuitOnLastWindowClosed ( false );

    QLauncher launcher;
    //    QLaunchManager launchmanager;


    bool btemp=launcher.startPlugin();
    // QMessageBox::warning(NULL,"loadok",btemp?"ok":"not ok");
    //   w.show();
    qDebug()<<"before exec";
    int ret;
    //try{
    ret=(!btemp)?0:(a.exec());
    //}
    ////catch(...)
    //{
    //	ASSERT(0);
    //}

#ifdef _WIN32

    try{
        SystemParametersInfo(SPI_SETFOREGROUNDLOCKTIMEOUT, 0, &lockTime, SPIF_SENDWININICHANGE | SPIF_UPDATEINIFILE);
    }
    catch(...)
    {
        Q_ASSERT(0);
    }

#endif
    //    launcher.stopPlugin();
    return ret;


}
