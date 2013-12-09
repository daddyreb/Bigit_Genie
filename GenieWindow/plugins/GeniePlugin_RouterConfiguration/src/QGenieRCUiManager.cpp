#include "QGenieRCUiManager.h"
#include <QtGui>
#include "QGenieRCMainWnd.h"

#define WIND_HANDLE "handle"

QGenieRCUiManager::QGenieRCUiManager(QObject *parent) :
    QObject(parent)
{

}

void QGenieRCUiManager::createWidgets()
{
    qDebug()<<"QGenieRCUiManager::createWidgets()";
    QGenieRCMainWnd *w=new QGenieRCMainWnd();
    w->setProperty(WIND_HANDLE,"/genie/stack/1");
}
