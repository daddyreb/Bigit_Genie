#include "qgeniefeedbackplugin.h"
#include "version.h"
#include <QtGui>
#include "workpanel.h"

//#define TEST_FORM_UI "X:\\Genie1.2\\trunk\\testplugin1\\TestForm.ui"

QGenieFeedBackPlugin::QGenieFeedBackPlugin()
    :mTestForm(NULL)
{

}
QVariant QGenieFeedBackPlugin::command(int cmd,const QVariant &paras)
{
//    HANDLER_CMD(0,QString());
    if(cmd ==I2CMD_UUID)
    {
        return QString(PLUGIN_UUID_FEEDBACK);
    }
    else if(cmd==I2CMD_VERSIONNUMBER)
    {
        return QString(VERSION_STRING);
    }
    else if(cmd==I2CMD_TIMETO_CREATEUI)
    {
        WorkPanel *workpanel = new WorkPanel(NULL);
        GENIE2_REGISTER_UI(workpanel,"1900:1900:FeedBack:cmd");
    }
    else if(cmd==I2CMD_TIMETO_START)
    {
    }
    else if(cmd ==I2CMD_UI_ACTION || cmd==I2CMD_UI_LOWER)
    {
    }
    else if(cmd ==I2CMD_NOTIFY_MESSAGE)
    {
        GENIE2_STATISTICS("i");
        qDebug()<<"I2CMD_NOTIFY_MESSAGE"<<paras;
    }
    return QVariant();
}

Q_EXPORT_PLUGIN2(GeniePlugin_FeedBack,QGenieFeedBackPlugin)
