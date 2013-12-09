#include "GeniePlugin_Test.h"
#include "version.h"
#include "genie2_interface.h"
#include "QGenieTestForm.h"
#include <QtGui>

//#define TEST_FORM_UI "X:\\Genie1.2\\trunk\\testplugin1\\TestForm.ui"


GeniePlugin_Test::GeniePlugin_Test()
    :mTestForm(NULL)
{
//	int *itemp=new int;
}

GeniePlugin_Test::~GeniePlugin_Test()
{
//    GENIE2_SAFEDELETE(mTestForm);
}

QVariant GeniePlugin_Test::command(int cmd,const QVariant &paras)
{
//    HANDLER_CMD(0,QString());
    if(cmd ==I2CMD_UUID)
    {
        return QString(PLUGIN_UUID);
    }
    else if(cmd==I2CMD_VERSIONNUMBER)
    {
        return QString(VERSION_STRING);
    }
    else if(cmd==I2CMD_TIMETO_CREATEUI)
    {
        QLabel *fake_form=new QLabel();
        fake_form->setText("fake_form");
        GENIE2_REGISTER_UI(fake_form,"1903:1900:Map:cmd1");
        GENIE2_REGISTER_UI(fake_form,"1904:1901:TestIco:cmd2");
        GENIE2_SUBSCRIB_MESSAGE(SCCMD_LANGUAGE);

    }
    else if(cmd==I2CMD_TIMETO_START)
    {
        GENIE2_UI_COMMAND(UICMD_LEFTBUTTON_LANGUAGE,1906);
        Q_ASSERT(!mTestForm);
        mTestForm=new QGenieTestForm();
        GENIE2_REGISTER_DBG_UI(mTestForm);
//        GENIE2_SET_FRAMELESSSTYLE(mTestForm);
//		mTestForm->show();
    }
    else if(cmd ==I2CMD_UI_ACTION || cmd==I2CMD_UI_LOWER)
    {
        qDebug()<<"ui action"<<cmd;
    }
    else if(cmd ==I2CMD_NOTIFY_MESSAGE)
    {
        qDebug()<<"I2CMD_NOTIFY_MESSAGE"<<paras;
       // GENIE2_STATISTICS("i");
    }
    return QVariant();
}



Q_EXPORT_PLUGIN2(GeniePlugin_Test, GeniePlugin_Test)
