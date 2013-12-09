#include "GeniePlugin_Update.h"
#include "version.h"
#include "QGenieUpdateObject.h"

#include <QtGui>



GeniePlugin_Update::GeniePlugin_Update()
    :mSoftObj(new QGenieUpdateObject)
{
}

GeniePlugin_Update::~GeniePlugin_Update()
{
    GENIE2_SAFEDELETE(mSoftObj);
}
////////////////////////////////////////////////////////////
//        NAME: command()
// DESCRIPTION: .
//   ARGUMENTS: int cmd, const QVariant &paras
//     RETURNS: void
//      AUTHOR: Chensi
//        DATA: 2012-2-8
////////////////////////////////////////////////////////////
QVariant GeniePlugin_Update::command(int cmd, const QVariant &paras)
{
    if(cmd == I2CMD_UUID)
    {
        return QString(PLUGIN_UUID);
    }
    else if(cmd==I2CMD_VERSIONNUMBER)
    {
        return QString(VERSION_STRING);
    }
    else if(cmd==I2CMD_TIMETO_CREATEUI)
    {
        //if(!mFirmwareUpdate)
        //{
        //    mFirmwareUpdate = new GeniePlugin_Firmware_Update();
        //}
        ////QString ver = "JWNR2000LV";
        //GENIE2_REGISTER_UI(mFirmwareUpdate, "1950:1950:Map:Update");
        //GENIE2_SUBSCRIB_MESSAGE(SCCMD_ROUTER_MODEL);
    }
    else if(cmd==I2CMD_TIMETO_START)
    {

    }
    else if(cmd ==I2CMD_UI_ACTION)
    {

    }
    else if(cmd ==I2CMD_NOTIFY_MESSAGE)
    {
    }
    else if(cmd == I2CMD_ABOUTTO_QUIT)
    {
	}
    else if(cmd==EPCC_ROUTERCONFIGUI)
    {
        QWidget *w=new QWidget();

        QVariant v;
        v.setValue((void *)w);

        return v;
    }
    else if(cmd == EPCC_SOFTWARE_UPDATE)
    { 
        mSoftObj->openDialog();
    }
    else if(cmd == EPCC_FIRMWARE_UPDATE)
    {

    }
    return QVariant();
}




