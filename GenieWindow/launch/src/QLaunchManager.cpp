#include "QLaunchManager.h"
#include "QLaunchUpdateDialog.h"
#include "defines.h"
#include "setting_def.h"
#ifdef _WIN32
#include"wintool.h"
#endif
#include "QUpdateProcess.h"
#include "QLauncher.h"



QLaunchManager::QLaunchManager()
#ifndef SHEET_STYLE_FORMAC
    :mDialog(NULL)
#else
    :mDialog(new QLaunchUpdateDialog())
#endif
    ,mQuitFlag(false)
    ,mProcess(NULL)
{
}
QLaunchManager::~QLaunchManager()
{
    SAFE_DELETE(mProcess);
#ifndef SHEET_STYLE_FORMAC
    SAFE_DELETE(mDialog);
#endif
}

bool QLaunchManager::startUpdate(UPDATE_RESON reson,QString lan
#ifdef SHEET_STYLE_FORMAC
                             ,QWidget *parent
#else
#endif
                             )
{
    qDebug() << "start update";
    mGenieLanCache=lan;
    static bool sDialogOn=false;
    if(reson == UR_USER_CHOISE)
    {
        if(mDialog && mDialog->isVisible())
        {
            return false;
        }
        sDialogOn=true;
	SAFE_DELETE(mDialog);

        QUpdateProcess *process=QUpdateProcess::GetSingletonPtr();


        ASSERT(process == mProcess);
        if(process == mProcess)
        {
            SAFE_DELETE(mProcess);
        }
#ifdef SHEET_STYLE_FORMAC
        SAFE_DELETE(mDialog);
        mDialog=new QLaunchUpdateDialog(parent);
        mDialog->changeText();
#else
        if(!mDialog)
        {
            mDialog=new QLaunchUpdateDialog();
            mDialog->changeText();
        }
#endif

#ifdef SHEET_STYLE_FORMAC
        mDialog->setWindowFlags( Qt::Sheet| Qt::FramelessWindowHint);
#else
        const Qt::WindowFlags cFlags[]={Qt::Dialog| Qt::FramelessWindowHint,DIALOG_FLAG};
        mDialog->setWindowFlags(cFlags[(int)reson]);
        mDialog->setModal(true);
#endif


        mDialog->startup(reson);




        bool ret=mDialog->mMustRestart;
        sDialogOn=false;
        return ret;
    }
    else if(reson == UR_TIMER_TRIGGER )
    {

        if(!sDialogOn)
        {
///////////this code used to fix crash bug//////////////
            QLauncher *launcher=QLauncher::GetSingletonPtr();
            ILaunchInf *li=NULL;
            if(launcher)
            {
                li=launcher->getLaunchInf();
            }
            if(!li)
            {
                return false;
            }
//////////////////////////////////////

            SAFE_DELETE(mDialog);
            QUpdateProcess *process =QUpdateProcess::GetSingletonPtr();
            ASSERT(process == mProcess);
            if(!process)
            {
                mProcess=new QUpdateProcess(NULL);
                mProcess->startup(UR_TIMER_TRIGGER);
            }
        }
        else
        {

        }
    }
    return false;
}



void QLaunchManager::setSetting(QString para,QVariant value)
{
    //mSettings.setValue(para,value);
}

#ifdef _WIN32
bool QLaunchManager::elevateThis()
{
    return elevate();
}

#endif
