#include "GeniePlugin_About.h"
#include <QtPlugin>
#include <QLabel>
#include <QHBoxLayout>
#include "IManagerHandler.h"
#include "GenieAboutForm.h"
#include <QAction>
#include "ITrayManager.h"

#define PLUGIN_INFO "UUID:{8c67a28b-aab2-4d27-a837-766d9337787f};\
BIND:1;\
            HAVEWND:1;\
                VERSION:GeniePlugin_About_V001"

GeniePlugin_About::GeniePlugin_About()
{
}

QString GeniePlugin_About::text()
{
    return QString("About");
}

QWidget* GeniePlugin_About::widget(QWidget *parent)
{
    GenieAboutForm *w=new GenieAboutForm(parent);

    return w;
}

QIcon * GeniePlugin_About::icon()
{
    return NULL;
}

int GeniePlugin_About::changelanguage(int idx)
{
	return 0;
}

QString  GeniePlugin_About::info()
{
    return QString(PLUGIN_INFO);
}

int GeniePlugin_About::priority()
{
    return 3000;
}


void GeniePlugin_About::setHandler(IManagerHandler *hdl)
{
    GenieAboutForm::mHandler=hdl;
}

void GeniePlugin_About::startup()
{
    QWidget *w=new QWidget();
    w->resize(100,100);
    w->show();
    QAction *act=new QAction("test",w);
    ITrayManager *traymanager=GenieAboutForm::mHandler->getTrayManager();
    traymanager->setAction(act);
}

void GeniePlugin_About::notifyRaise()		//when plugin raise to top of stack,plugin may catch it to start/stop some processes
{

}
void GeniePlugin_About::notifyLower()		//opposite to notifyRaise
{

}
int GeniePlugin_About::getStatus()			//return the status of plugin,refer to ENUM_PLUGIN_STATUS
{
    return 0;
}

void GeniePlugin_About::notifyTrayMessage(QSystemTrayIcon::ActivationReason reason,MAIN_FRAME_RESPONSE res)
{

}






Q_EXPORT_PLUGIN2(GeniePlugin_About, GeniePlugin_About)
