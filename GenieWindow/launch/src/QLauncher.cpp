#include "QLauncher.h"
#include <QFileInfo>
#include <QDir>
//#include "defines.h"
//#include "ILaunchInf.h"
#include "genie2_interface.h"
//#include "QGenieSingleProcessHandler.h"
//#include "defines.h"
//#include "QLaunchManager.h"
//#include "QLaunchUpdateDialog.h"
// 
#include <QSettings>
#include <QDebug>
#include <QDesktopServices>
#include <QUrl>
#include <QApplication>
//#include "QGenieSettingHelper.h"
#include <QDate>
#include <QTime>
#include <QMessageBox>
#ifdef Q_OS_MAC
#define GENIE_DLL "libGenie.dylib"
#define PLUGIN_DIR (QDir(qApp->applicationDirPath()+"/../PlugIns"))
#else
#define GENIE_DLL "Genie.dll"
#define PLUGIN_DIR (QDir(qApp->applicationDirPath()))
#endif

//extern bool gMini;



QLauncher::QLauncher()
//    :mLi(NULL)
//    ,mSingleHandler(new QGenieSingleProcessHandler())
{
    ///connect(&mUpdater,SIGNAL(signal_Finished(bool)),this,SLOT(slot_UpdateProcessFinished(bool)));

//	connect(&mDebugTimer,SIGNAL(timeout()),this,SLOT(slot_Debug()));
}
QLauncher::~QLauncher()
{
#ifndef COMPLETE_EXIT
    if(!QApplication::arguments().contains("-dbg"))
    {
        _exit(0);
    }
    else
    {
        loader.unload();
    }
#else
    loader.unload();
#endif
//    SAFE_DELETE(mSingleHandler);
}

//bool QLauncher::tryStartup()
//{
//    if(mSingleHandler)
//    {
//       return mSingleHandler->tryCreate();
//    }
//    return true;
//}

bool QLauncher::startPlugin()
{


//    if(!QFile::exists(CHANGABLE_INI_FNAME))
//    {
//        INIT_ALL_SETTINGS();
//    }
//    if(!QFile::exists(PERMANENT_INI_FNAME))
//    {
//        INIT_ALL_SETTINGS_PERMANENT();
//    }
//    slot_RealStartup();

//    QMessageBox::warning(NULL,"exist",PLUGIN_DIR.exists(GENIE_DLL)?"true":"false");
     if(PLUGIN_DIR.exists(GENIE_DLL))
    {
    //    mLoader.unload();

        loader.setFileName(PLUGIN_DIR.path()+"/"+GENIE_DLL);
        QObject *obj=loader.instance();
        if( obj->property("exitflag").toBool())
        {
            return false;
        }
        IPluginInf2 *li=qobject_cast<IPluginInf2 *>(obj);
        return (li !=NULL);
    }
     return false;
}

//void QLauncher::slot_RealStartup()
//{

//}

//void QLauncher::debugRestart()
//{
////	mDebugTimer.start(2000);

//}

//QString QLauncher::cmd(QString para)
//{
//    return mLi?(mLi->command(para)):"no plugin";
//}

//void QLauncher::stopPlugin()
//{
//	qDebug()<<"QLauncher stop plugin";
//    if(mLi)
//    {
//        mLi->endup();
//    }
//    mLi=NULL;
//    bool btemp=mLoader.unload();
//    qDebug()<<"unload genie.dll"<<btemp;
//}




//void QLauncher::slot_Debug()
//{
//	mDebugTimer.stop();
////	stopPlugin();

//        startPlugin();
//	//                        e->accept();
//	//                        return true;

//}






