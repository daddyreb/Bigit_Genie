#include "GeniePlugin_Airprint.h"
#include <QtPlugin>
#include <QLabel>
#include <QHBoxLayout>
#include <QAction>
#include "language_AirPrint.h"
#include "version.h"

GeniePlugin_Airprint::GeniePlugin_Airprint()
{
}

GeniePlugin_Airprint::~GeniePlugin_Airprint()
{
}

QVariant GeniePlugin_Airprint::command(int cmd,const QVariant &paras)
{
    if(cmd == I2CMD_UUID){
          return QString(PLUGIN_UUID);
      }
      else if(cmd == I2CMD_VERSIONNUMBER){
          return QString(VERSION_STRING);
      }
      else if(cmd == I2CMD_TIMETO_CREATEUI){
          w = new AirprintWidget();
          GENIE2_REGISTER_UI(w,QString("1750:") + QString::number(L_AIRPRINT_BIGBT) + ":Airprint:cmd1");
      }
      return QVariant();
}


