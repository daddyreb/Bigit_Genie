#include "GeniePlugin_Statistics.h"
#include <QtPlugin>
#include <QLabel>
#include <QHBoxLayout>
#include <QPushButton>
#include <QAction>
#include <QDebug>
#include "version.h"
#include <qdatetime.h>
#include "QGenieDataContainner.h"
#include "QGenieStaticInterface.h"

#define PLUGIN_INFO "UUID:%1;\
BIND:1;\
HAVEWND:0;\
VERSION:%2"


GeniePlugin_Statistics::GeniePlugin_Statistics()
    :mContainner(NULL)
    ,mInterface(NULL)
{
    mContainner=new QGenieDataContainner();
    mInterface=new QGenieStaticInterface();
}

GeniePlugin_Statistics::~GeniePlugin_Statistics()
{
    GENIE2_SAFEDELETE(mContainner);
    GENIE2_SAFEDELETE(mInterface);
}

QVariant GeniePlugin_Statistics::command(int cmd, const QVariant &paras)
{

    QStringList tmp = paras.toStringList();
    QString para0 = tmp.value(0);
    QString para1 = tmp.value(1);
    if(cmd == I2CMD_OTHER1)
    {
        mInterface->show();
        mInterface->updateUi();

    }
    else if(cmd == I2CMD_UUID)
    {
        return QString(PLUGIN_UUID);
    }
    else if(cmd==I2CMD_VERSIONNUMBER)
    {
        return QString(VERSION_STRING);
    }
    else if(cmd==I2CMD_TIMETO_CREATEUI)
    {
    }
    else if(cmd==I2CMD_TIMETO_START)
    {
        GENIE2_REGISTER_DBG_UI(mInterface);
    }
    else if(cmd ==I2CMD_UI_ACTION)
    {
        qDebug()<<"ui action";
    }
    else if(cmd ==I2CMD_NOTIFY_MESSAGE)
    {
        qDebug()<<"I2CMD_NOTIFY_MESSAGE"<<paras;
    }
    else if(cmd == I2CMD_STATISTICS)
    {
        ////add click
        //if(para1.isEmpty() || para1 == "0")
        //{
        //    /////add click
        //    mContainner->addClick(para0);
        //}
        if(para1 == "1")
        {
            //////add click

            mContainner->addClick(para0);

        }
        else if(para1 == "2")
        {
            mContainner->addSearchStr(para0);
        }
        else if(para1 == "3")
        {
            //reg max value
            bool btemp;
            int itemp = para1.toInt(&btemp);
            if(btemp)
            {
                mContainner->addClick(para0, AT_REGMAX, itemp);
            }
        }
		else
		{
			/////add click

		}
    }
    else if(cmd == I2CMD_ABOUTTO_QUIT)
    {
        GENIE2_SAFEDELETE(mContainner);
        GENIE2_SAFEDELETE(mInterface);
    }
    return QVariant();
}



