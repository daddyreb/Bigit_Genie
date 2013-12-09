#include "GeniePlugin_Resource.h"
#include "version.h"
#include "genie2_interface.h"



GeniePlugin_Resource::GeniePlugin_Resource()
{

}
QVariant GeniePlugin_Resource::command(int cmd,const QVariant &paras)
{
//    HANDLER_CMD(0,QString());
    Q_UNUSED(paras)
    if(cmd ==I2CMD_UUID)
    {
        return QString(PLUGIN_UUID);
    }
    else if(cmd==I2CMD_VERSIONNUMBER)
    {
        return QString(VERSION_STRING);
    }
    return QVariant();
}




