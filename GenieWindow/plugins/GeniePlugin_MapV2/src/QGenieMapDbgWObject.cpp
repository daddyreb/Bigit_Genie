#include "QGenieMapDbgWObject.h"
#include <QUiLoader>
#include "genie2_interface.h"
#include "topology.h"

extern quint32 sMapFlags;
QGenieMapDbgWObject::QGenieMapDbgWObject(QObject *parent)
    :QObject(parent)
      ,mW(NULL)
{
}

QWidget *QGenieMapDbgWObject::getW()
{
    if(!mW)
    {
        bool btemp;
        QUiLoader loader;
        QFile f(GENIE2_RES("ui/mapdbgW.ui"));
        btemp=f.open(QFile::ReadOnly);
        Q_ASSERT(btemp);
        QWidget *w=loader.load(&f, NULL);
        Q_ASSERT(w);

//        deployIndicators(w);

        QList<QCheckBox*> bts=w->findChildren<QCheckBox *>();
        int c=bts.count();
        for(int i=0;i<c;i++)
        {
            connect(bts[i],SIGNAL(clicked(bool)),this,SLOT(slot_RdtClicked(bool)));
        }
        mW=w;

    }
    return mW;
}

void QGenieMapDbgWObject::slot_RdtClicked(bool checked)
{
    QCheckBox *rdt=qobject_cast<QCheckBox *>(sender());
    Q_ASSERT(rdt);
    QString objname=rdt->objectName();
    qDebug()<<objname;

    struct {const char *name;quint32 flag;} cParas[]={
        {"arp",MF_USE_ARP_SCAN}
        ,{"netbios",MF_USE_NETBIOS_SCAN}
        ,{"rdp",MF_USE_RDP_SCAN}
        ,{"putty",MF_USE_PUTTY_SCAN}
        ,{"afp",MF_USE_AFP_SCAN}
        ,{"upnp",MF_USE_UPNP_SCAN}
        ,{"netgear",MF_USE_NETGEARSPECIFIC_SCAN}
    };
    int c=sizeof(cParas)/sizeof(cParas[0]);
    for(int i=0;i<c;i++)
    {
        if(objname==cParas[i].name)
        {
            if(checked)
            {
                sMapFlags |= cParas[i].flag;
            }
            else
            {
                sMapFlags &=~cParas[i].flag;
            }
        }
    }
}
