#include "GeniePluginTest.h"
#include "version.h"
#include "genie2_interface.h"
#include <QtGui>
#include "mainwindow.h"

GeniePlugin_NetWorkProblem::GeniePlugin_NetWorkProblem()
{

}
QVariant GeniePlugin_NetWorkProblem::command(int cmd,const QVariant &paras)
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
        MainWindow *w=new MainWindow();
        QList<QPushButton *> buts=w->findChildren<QPushButton *>();
        int c=buts.count();
        for(int i=0;i<c;i++)
        {
            buts[i]->setFlat(true);
        }
        GENIE2_REGISTER_UI(w,"1800:1800:Network_Problem:cmd1");
    }
    else if(cmd==I2CMD_TIMETO_START)
    {
    }
    else if(cmd ==I2CMD_UI_ACTION || cmd==I2CMD_UI_LOWER)
    {
    }
    else if(cmd ==I2CMD_NOTIFY_MESSAGE)
    {
    }
    return QVariant();
}




