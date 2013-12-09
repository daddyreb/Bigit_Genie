#include "QGenieTrayManager.h"
#include "QGenieTrayIco_In.h"
//#include "genie2_interface.h"
//#include "QGenieDialogExit.h"
#include <QUiLoader>
#include <QDomDocument>
#include <QtGui>
#include "QGenieSelfTrayMessage.h"

#define MESSAGE_SHOWING_TIME 3000
//#define TRAY_EXIT_UI "X:\\Genie1.2\\trunk\\testplugin_ui\\ui\\QGenieDialogExit.ui"

namespace Ui
{
    class QGenieSelfTrayMessage;
}

QGenieTrayIco_In::QGenieTrayIco_In(QGenieTrayManager *manager)
    :mTrayIcon(new QSystemTrayIcon())
     ,mTrayManager(manager)
    ,mTrayMenu(NULL)
   // ,mDialogExit(NULL)
    ,mSelfTrayMessage(new QGenieSelfTrayMessage())
{
#if 0
    QUiLoader loader;
    QFile uifile(TRAY_EXIT_UI);
    bool btemp=uifile.open(QFile::ReadOnly);
    Q_ASSERT(btemp);
    mDialogExit=loader.load(&uifile, NULL);
    Q_ASSERT(mDialogExit);

    startup();
    QList<QPushButton *> buttons=mDialogExit->findChildren<QPushButton *>();//qFindChildren<QPushButton *>(mDialogExit);
    foreach(QPushButton *but,buttons)
    {
        connect(but,SIGNAL(clicked()),this,SLOT(slot_ButtonClicked()));
    }

    QList<QAction *> acts=mTrayMenu->findChildren<QAction*>();
    foreach(QAction *act,acts)
    {
        connect(act,SIGNAL(triggered(bool)),this,SLOT(slot_ActChecked(bool)));
    }
#endif

   // QTimer::singleShot(0,this,SLOT(slot_EmitInitMessage()));
    QTimer::singleShot(0,mTrayManager,SIGNAL(canInitNow()));
}

QGenieTrayIco_In::~QGenieTrayIco_In()
{
    mTrayIcon->hide();
    delete (mTrayIcon);
    delete ( mTrayMenu);
    delete (mSelfTrayMessage);
}

//void QGenieTrayIco_In::slot_ButtonClicked()
//{

//}

//void QGenieTrayIco_In::slot_ActChecked(bool)
//{

//}
QMenu * QGenieTrayIco_In::_createMenu(const QDomElement &domelem)
{
    qDebug()<<domelem.attribute("title");
    QMenu *menu=new QMenu();
    QString objname=domelem.attribute("name");
    if(!objname.isEmpty())
    {
        menu->setObjectName(objname);
    }
    menu->setTitle(domelem.attribute("title"));

    QDomNodeList nodelist=domelem.childNodes();
    int c=nodelist.count();
    for(int i=0;i<c;i++)
    {
        QDomNode node=nodelist.item(i);
        QDomElement nodeelem=node.toElement();
        qDebug()<<nodeelem.tagName();
        if(nodeelem.isNull())
        {
            continue;
        }
        if((nodeelem.tagName().compare("widget",Qt::CaseInsensitive)==0)
            &&(nodeelem.attribute("class").compare("QMenu",Qt::CaseInsensitive)==0))
            {
            QMenu *submenu=_createMenu(nodeelem);
            menu->addMenu(submenu);
//			submenu->setParent(menu);
			
        }
        else if(nodeelem.tagName().compare("action")==0)
        {
            QAction *act=menu->addAction(nodeelem.attribute("title"));
            mActCache.append(act);
            if(nodeelem.attribute("checkable")=="1")
            {
                act->setCheckable(true);
            }
//			act->setParent(menu);
            QString stemp=nodeelem.attribute("name");
            if(!stemp.isEmpty())
            {
                act->setObjectName(stemp);
            }
            stemp=nodeelem.attribute("icon");
            if(!stemp.isEmpty())
            {
                act->setIcon(QIcon(stemp));
            }
        }
        else if(nodeelem.tagName().compare("spacer")==0)
        {
            menu->addSeparator();
        }
    }
    mMenuCache.append(menu);
    return menu;

}

QMenu *QGenieTrayIco_In::findMenu(const QString &objname)
{
    foreach(QMenu *menu,mMenuCache)
    {
        if(menu->objectName()==objname)
        {
            return menu;
        }
    }
    return NULL;
}

QAction *QGenieTrayIco_In::findAction(const QString &objname)
{
    foreach(QAction *act,mActCache)
    {
        if(act->objectName()==objname)
        {
            return act;
        }
    }
    return NULL;

}

void QGenieTrayIco_In::createTrayMenu(const QString &uiFName)
{
//    Q_ASSERT(!mTrayMenu);
    if(mTrayMenu)
    {
        delete mTrayMenu;
    }
    mMenuCache.clear();
    mActCache.clear();
    QFile f(uiFName);
    if(f.open(QIODevice::ReadOnly))
    {
        QDomDocument xmlDoc;
        xmlDoc.setContent(&f);
        QDomNode root = xmlDoc.firstChild();
        if (!root.isElement())
            root = root.nextSibling();
        QDomElement rootelem=root.toElement();
        qDebug()<<rootelem.tagName()<<rootelem.attribute("class");
        if((rootelem.tagName().compare("widget",Qt::CaseInsensitive)==0)
            && (rootelem.attribute("class").compare("QMenu",Qt::CaseInsensitive)==0)
            )
            {
            QMenu *menu=_createMenu(rootelem);
            menu->move(300,300);
//            menu->exec();
            mTrayMenu=menu;

            mTrayIcon->setContextMenu(mTrayMenu);
            connect(mTrayMenu,SIGNAL(triggered(QAction*)),this,SLOT(slot_ActionTrigged(QAction*)));
            connect(mTrayMenu,SIGNAL(aboutToShow()),this,SLOT(slot_AboutToShow()));
            connect(mTrayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(slot_TrayActivated(QSystemTrayIcon::ActivationReason)));
        }

    }





    //    qDebug()<<rootelem.isNull()<<rootelem.text()<<rootelem.tagName()<<rootelem.attribute("class")<<rootelem.attribute("name");
    //    qDebug()<<root.toElement().tagName();
    //    QDomNodeList items=root.childNodes();
}

void QGenieTrayIco_In::translateUi(const QString &paras)
{
    //todo choose a gooder spliter
    QStringList lst=paras.split(",");
    QRegExp reg("(\\s*)(.*)\\[(.*)\\]=\"(.*)\"(\\s*)");
    int c=lst.count();
    for(int i=0;i<c;i++)
    {
        if(reg.exactMatch(lst[i]))
        {
            QString objname=reg.cap(2);
            QString classname=reg.cap(3);
            QString text=reg.cap(4);

            if(classname.compare("menu",Qt::CaseInsensitive)==0)
            {

                QMenu *menu=findMenu(objname);
                if(menu)
                {
                    menu->setTitle(text);
                }
            }
            else if(classname.compare("action",Qt::CaseInsensitive)==0)
            {
                QAction *act=findAction(objname);
                if(act)
                {
                    act->setText(text);
                }
            }
//            mTrayMenu->exec();
//            qDebug()<<reg.cap(0)<<reg.cap(1)<<reg.cap(2)<<reg.cap(3);
        }

    }
}

void QGenieTrayIco_In::setActCheckState(const QString &paras)
{

    QStringList lst=paras.split(",");
    QRegExp reg("(\\s*)(.*)=(\\d)(\\s*)");
    int c=lst.count();
    for(int i=0;i<c;i++)
    {
        if(reg.exactMatch(lst[i]))
        {
            QString objname=reg.cap(2);
            bool state=(reg.cap(3)=="1");
            QString text=reg.cap(4);
            QAction *act=findAction(objname);
            if(act)
            {
                act->setChecked(state);
            }
        }

    }
}

void QGenieTrayIco_In::uiCommand(int cmd,  const QString &para)
{
    QPoint p;
    QFile f;
    QString stemp;
    switch(cmd)
    {
    case TUIC_CREATE_TRAY:
        createTrayMenu(para);
        break;
    case TUIC_SHOW_ICO:
        mTrayIcon->setIcon(QIcon(para));
        mTrayIcon->show();
        break;
    case TUIC_HIDE_ICO:
        mTrayIcon->hide();
        break;
    case TUIC_TRANSLATE_ACTION:
        translateUi(para);
        break;
    case TUIC_ACTION_STATE:
        setActCheckState(para);
        break;
    case TUIC_TRAY_MESSAGE:
        if(mTrayIcon->isVisible())
        {
            p=mTrayIcon->geometry().center();
            mSelfTrayMessage->setPosition(p.x(),p.y());
            mSelfTrayMessage->showText(para);
            mSelfTrayMessage->show();
        }
        break;
    case TUIC_SET_QSS:
        f.setFileName(para);
        if(f.open(QIODevice::ReadOnly))
        {
            stemp=f.readAll();
            foreach(QMenu *menu,mMenuCache)
            {
                menu->setStyleSheet(stemp);
            }

//            mTrayMenu->setStyleSheet(f.readAll()   );
        }
        break;
    case TUIC_SET_TOOLTIP:
        mTrayIcon->setToolTip(para);
        break;
    case TUIC_APP_QUIT:

        break;
    }
}


void QGenieTrayIco_In::slot_AboutToShow()
{
    mTrayMenu->setWindowOpacity(1.0);
}

void QGenieTrayIco_In::slot_ActionTrigged(QAction *act)
{
    Q_ASSERT(mTrayManager);
    if(act )
    {
        QString signalPara=act->objectName()+","+(act->isChecked()?"1":"0");
        emit mTrayManager->actionTrigged(signalPara);
    }
}




void QGenieTrayIco_In::slot_TrayActivated(QSystemTrayIcon::ActivationReason reson)
{

    Q_ASSERT(mTrayManager);
    if(reson == QSystemTrayIcon::DoubleClick)
    {
        QString signalPara = "doubleClick";
        signalPara += ",";
        signalPara += "1";
        emit mTrayManager->actionTrigged(signalPara);
    }
    else if(reson == QSystemTrayIcon::Trigger)
    {
        QString signalPara = "trigged";
        signalPara += ",";
        signalPara += "1";
        emit mTrayManager->actionTrigged(signalPara);
    }
}
//void QGenieTrayIco_In::slot_EmitInitMessage()
//{
//    emit mTrayManager->canInitNow();
//}
