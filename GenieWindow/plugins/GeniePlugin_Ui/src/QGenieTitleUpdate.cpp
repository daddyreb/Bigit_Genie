#include "QGenieTitleUpdate.h"
#include <QObject>
#include "genie2_interface.h"
#include "QGenieUiObject.h"
#include "language_Ui.h"
#include <QDesktopServices>

#define SOFT_FIRMWARE_CHANGE_INTERVAL 30 //sec

enum UPDATE_STATE
{
    SOFTWARE_UPDATE=(1<<0),
    FIRMWARE_UPDATE=(1<<1),
    RSVALT_UPDATE=(1<<2)
};

static int flagOneCount(quint32 a)
{
    int count=0;
    while(a>0)
    {
        a=(a-1)&a;
        count++;
    }
    return count;
}

QGenieTitleUpdate::QGenieTitleUpdate(QObject *parent)
    :QObject(parent)
    ,mUpdatFuncFlag(0)
    ,mCurShowingFlag(0)
{

    connect(&mTimerSoftwareOrFirmware, SIGNAL(timeout()), this, SLOT(slot_SoftwareOrFirmwareTimeout()));

}

QGenieTitleUpdate::~QGenieTitleUpdate(void)
{
    disconnect(&mTimerSoftwareOrFirmware,SIGNAL(timeout()),NULL,NULL);
    mTimerSoftwareOrFirmware.stop();
}

void QGenieTitleUpdate::command(const QString &cmd)
{
    quint32 savedFlags=mUpdatFuncFlag;
    if(cmd=="soft_on")
    {
        mUpdatFuncFlag |= SOFTWARE_UPDATE;
    }
    else if(cmd=="soft_off")
    {
        mUpdatFuncFlag &= ~SOFTWARE_UPDATE;
    }
    else if(cmd=="firm_on")
    {
        mUpdatFuncFlag |= FIRMWARE_UPDATE;
    }
    else if(cmd=="firm_off")
    {
        mUpdatFuncFlag &= ~FIRMWARE_UPDATE;
    }
#ifdef Q_OS_WIN
    else if(cmd=="valt_on")
    {
        mUpdatFuncFlag |= RSVALT_UPDATE;
    }
    else if(cmd=="valt_off")
    {
        mUpdatFuncFlag &= ~RSVALT_UPDATE;
    }
#endif

    if(savedFlags != mUpdatFuncFlag)
    {
        mTimerSoftwareOrFirmware.stop();
        slot_SoftwareOrFirmwareTimeout();
        if(flagOneCount(mUpdatFuncFlag)>1)
        {
            mTimerSoftwareOrFirmware.start(SOFT_FIRMWARE_CHANGE_INTERVAL * 1000);
        }
    }

}

void QGenieTitleUpdate::slot_SoftwareOrFirmwareTimeout()
{
    if(mUpdatFuncFlag)
    {
        if(!mCurShowingFlag)
        {
            mCurShowingFlag=1;
        }
        const quint32 cF=FIRMWARE_UPDATE|SOFTWARE_UPDATE|RSVALT_UPDATE;
        do{
            mCurShowingFlag=(mCurShowingFlag<<1)&cF;
            mCurShowingFlag=(!mCurShowingFlag)?1:mCurShowingFlag;
        }while(0==(mCurShowingFlag&mUpdatFuncFlag));
    }  

    emit updateIcon();
}

void QGenieTitleUpdate::butClicked()
{
//    Q_ASSERT(mTitleUpdate);
    if(mCurShowingFlag&SOFTWARE_UPDATE)
    {
        QGenieUiObject *uiobj=QGenieUiObject::GetSingletonPtr();
        Q_ASSERT(uiobj);
        GENIE2_TELL_OTHERPLUGIN(PLUGIN_UUID_UPDATE, EPCC_SOFTWARE_UPDATE, uiobj->mCurLan);
    }
    else if(mCurShowingFlag&FIRMWARE_UPDATE)
    {
        GENIE2_UI_COMMAND(UICMD_CHECK_LEFTBUT,PLUGIN_UUID_NETCONFIG);
        GENIE2_TELL_OTHERPLUGIN(PLUGIN_UUID_NETCONFIG, EPCC_FIRMWARE_UPDATE, 0);
        GENIE2_TELL_OTHERPLUGIN(PLUGIN_UUID_UPDATE, EPCC_FIRMWARE_UPDATE, 0);
    }
    else if(mCurShowingFlag & RSVALT_UPDATE)
    {
        QDesktopServices::openUrl(QUrl(GENIE2_GET_SETTING(RSVALT_LINK).toString()));
    }

    GENIE2_STATISTICS("H4");
}

QString QGenieTitleUpdate::getTitleText()
{
    if(!mUpdatFuncFlag)
    {
        return QString();
    }
    const struct _{quint32 flag;int lan;} cParas[]=
    {
        {SOFTWARE_UPDATE,L_UI_SOFTWARE_UPD_AVALABLE}
        ,{FIRMWARE_UPDATE,L_UI_FIRMWARE_UPD_AVALABLE}
        ,{RSVALT_UPDATE,L_UI_VALT_UDP_AVALABLE}
    };
    for(int i=0;i<sizeof(cParas)/sizeof(cParas[0]);i++)
    {
        if(cParas[i].flag& mCurShowingFlag)
        {
            return GENIE2_GET_TEXT(cParas[i].lan);
        }
    }
    return QString();
}







































