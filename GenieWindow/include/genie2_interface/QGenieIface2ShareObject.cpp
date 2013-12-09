#include "genie2_interface.h"
#include "version.h"
#include <QtCore>
#include <QtGui>
#include <QApplication>

class QGenieIface2ShareObject;
#define SHAREOBJ_NAME "Iface2SO"
QGenieIface2ShareObject *getSinglePtr()
{
    qDebug()<<"QGenieIface2ShareObject *getSinglePtr()";
    QObject* obj=qApp->findChild<QObject *>(SHAREOBJ_NAME);
    QGenieIface2ShareObject *ret=(QGenieIface2ShareObject *)obj;//qobject_cast<QGenieIface2ShareObject *>(obj);
    return ret;
}

QGenieIface2ShareObject::QGenieIface2ShareObject()
    :QObject(qApp)
  ,mHandler(NULL)
  ,mNetgearInfoM(NULL)
  ,mDBGRouteModel(QString())
  ,mMainFrame(NULL)
  ,mFlags(0)
{
    Q_ASSERT(sSingleInstance==NULL);
    this->setObjectName(SHAREOBJ_NAME);
    this->setParent(qApp);
    sSingleInstance=this;
    QRegExp reg("^--resource=(.*)");
    if(QApplication::arguments().indexOf(reg)>=0)
    {
        qDebug()<<reg.cap(0)<<reg.cap(1);
        mReSourcePrefix=reg.cap(1);
        if(mReSourcePrefix[mReSourcePrefix.length()-1]==QChar('\\'))
        {
            mReSourcePrefix.resize(mReSourcePrefix.length()-1);
        }
        mReSourcePrefix=mReSourcePrefix+"/";
        mReSourcePrefix.replace("\\","/");
    }
    else
    {
        mReSourcePrefix=":/";
    }
}

QGenieIface2ShareObject::~QGenieIface2ShareObject()
{

}

QString QGenieIface2ShareObject::getDbgResource(const QString &s)
{
    QString path=s;
    path.replace("\\","/");

    if((mReSourcePrefix[0]!=QChar(':')) && !QFile::exists(mReSourcePrefix+path))
    {
        return QString(":/")+path;
    }
    return mReSourcePrefix+path;
}

QGenieIface2ShareObject *QGenieIface2ShareObject::sSingleInstance=getSinglePtr();
int QGenieIface2ShareObject::sUuidIdx=(QGenieIface2ShareObject::sSingleInstance && QGenieIface2ShareObject::sSingleInstance->mHandler)?\
                                      (QGenieIface2ShareObject::sSingleInstance->mHandler->handler_command(HCMD_UUID_IDX,-1,QString(PLUGIN_UUID)).toInt()):-1;
