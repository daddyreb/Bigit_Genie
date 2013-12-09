#include "SvtFcml.h"
#include <QDomDocument>
#include <QApplication>
#include <QtCore>
#include "SvtNetworkTool.h"

static int sIdx=0;
//#define APPGENIE_STAG_NETGEAR_COM "appgenie-staging.netgear.com"

int SVT_NETWORKTOOL_EXPORT getRetValue(const QDomNode &node,QString &desp)
{
    desp.clear();
    bool btemp;

    QDomElement relem=node.toElement();
    QDomElement elem=node.firstChild().toElement();
//    qDebug()<<"getRetValue"<<elem.tagName();

    if((relem.tagName().compare("authenticate")==0 )
        &&(relem.attribute("result").toInt(&btemp)==401)
        && btemp
        )
    {
        desp=relem.attribute("description");
        return FE_NOT_AUTHENTICATIED;
    }

    QString stemp="portal.router.";
    if(
            (elem.tagName().left(stemp.length()).compare(stemp,Qt::CaseInsensitive)==0)
            && (elem.attribute("active").compare("false",Qt::CaseInsensitive)==0)
            )
    {

        //<portal.router.ui7237.is type="ui" active="false"/>
        desp="device not active";
        return FE_NOT_ACTIVE;
    }


    if(elem.tagName().compare("error",Qt::CaseInsensitive)==0)
    {

        int ret=elem.attribute("code").toInt(&btemp);
        desp=elem.attribute(/*"descrip"*/"description");
        if(ret==0 || !btemp)
        {
            ret=FE_ECODE_NOT_INT;
        }
        return ret;
    }
    if(elem.hasAttribute("_responseCode"))
    {
        int ret=elem.attribute("_responseCode").toInt(&btemp);
        if(!btemp)
        {
            ret=FE_RCODE_NOT_INT;
        }
        return ret;
    }
    return node.isNull()?FE_NULL_NODE:0;
}

QString cpName(const QString &s)
{
    //portal.router.cp171.is_all
    QRegExp m("portal\\.router\\.(.*)\\.is_all");
    if(m.exactMatch(s))
    {
        return m.cap(1);
    }
    return s;
}

void getCookie(const QByteArray &input,QMap<QString,QString> &output)
{
    output.clear();
    QString s=QString::fromLatin1(input,input.size());
    QRegExp m("(\r\n|\n){2,}");
    int itemp=m.indexIn(s);
    if(itemp >=0)
    {
        s=s.left(itemp);
    }
    QStringList slist=s.split(QRegExp("\r\n|\n"));
    QRegExp m1("Set-Cookie:\\s+(.*)=(.*);");
    m1.setMinimal(true);
    int c=slist.count();
    for(int i=0;i<c;i++)
    {
        if((itemp=m1.indexIn(slist[i]))>=0)
        {
            output.insert(m1.cap(1),m1.cap(2));
//            qDebug()<<m1.cap(1)<<m1.cap(2)<<m1.cap(3)<<m1.cap(4);
        }
    }


}


SVT_Fcml::SVT_Fcml(bool *exitFlag, QString url)
    :mExitFlag(exitFlag)
    ,mLinkUrl(url)
{
    ////////////

}

const QString &SVT_Fcml::appendCookie(QString &head)
{
    int c=mCookie.count();
    if(c>0)
    {
        head+="Cookie:";
        QMap<QString, QString>::const_iterator i;
        for (i = mCookie.constBegin(); i != mCookie.constEnd(); ++i)
        {
            head.append(QString(" %1=%2;").arg(i.key(),i.value()));
        }
        head.append("\r\n");
    }
    return head;

}

const QDomNode &SVT_Fcml::fcmlRequest(const QString &to, const QString &req)
{

    mNode=QDomNode();
    QString head="POST /fcp/send?n=%1 HTTP/1.1\r\n"
                 "Accept-Encoding: identity\r\n"
                 "Content-Length: %2\r\n"
                 "Host: %3\r\n"
                 "Connection: close\r\n"
                 "Content-Type: application/x-www-form-urlencoded\r\n";

    QString body="<fcml to=\"%1\" from=\"%2@portal\" _tracer=\"%3\">"
                 "%4</fcml>";
    int idx=++sIdx;
    body=body.arg(to,mName).arg(idx).arg(req);
    head=head.arg(mName).arg(body.size()).arg(mLinkUrl);
    SVT_Https http(mExitFlag);
    qDebug()<<body;
//    http.rawRequest(APPGENIE_STAG_NETGEAR_COM
//                    ,appendCookie(head)+"\r\n"+body);
    const QByteArray &array=http.rawRequest(mLinkUrl
            ,appendCookie(head)+"\r\n"+body);
    qDebug()<<array;



    QRegExp m("Content-Length: (\\d+)(\\s*)(\r\n|\n)");
    m.setMinimal(true);
    QString s=QString::fromLatin1(array.data(),array.size());
    int itemp;
    if( (itemp=m.indexIn(s)) >=0)
    {
//        qDebug()<<m.cap(1)<<m.cap(2);
        bool btemp;
        if( (0!=m.cap(1).toInt(&btemp) ) || !btemp)
        {
         //   mCache="post request not ok!!";
            QByteArray abody=SVT_Https::getBody(array);
            mDoc.clear();
            mDoc.setContent(abody);
            mNode=mDoc.firstChild();
            return mNode;
        }
    }

    //step2 get result
    int retryTimes=0;
    do
    {

        head="GET /fcp/receive?n=%1 HTTP/1.1\r\n"
                "Accept-Encoding: identity\r\n"
                "Host: %2\r\n"
                "Connection: close\r\n";
        head=head.arg(mName,mLinkUrl);
        const QByteArray &array1=http.rawRequest(mLinkUrl
                                                 ,appendCookie(head)+"\r\n");


        QByteArray abody=SVT_Http::getBody(array1);
        qDebug()<<abody;
        mDoc.clear();
        mDoc.setContent(abody,true);
        mNode = mDoc.firstChild();
        if (!mNode.isElement())
            mNode = mNode.nextSibling();
        if(!mNode.isElement())
        {
            return mNode;
        }


        if(0==mNode.toElement().tagName().compare("fcmb",Qt::CaseInsensitive))
        {
            mNode=mNode.firstChild();
            QDomNode node=mNode;
            while(!node.isNull())
            {
                if(node.isElement())
                {
                    QDomElement elem=mNode.toElement();
                    if( (0==elem.tagName().compare("fcml",Qt::CaseInsensitive))
                            && (elem.attribute("_tracer")==QString::number(idx))
                            && (elem.attribute("to").contains(mName))
                            )
                    {
                        mNode=node;
                    }
                }
                node=node.nextSibling();
            }
        }

        bool btemp;
        int tracer=mNode.toElement().attribute("_tracer").toInt(&btemp);

        if(!btemp || (tracer != idx))
        {
            mNode=QDomNode();
        }
        if(btemp && (tracer>=idx))
        {
            break;
        }

    }while((retryTimes++)<5);
    return mNode;
}

const QDomNode &SVT_Fcml::fcmlReqWithRefreshSession(const QString &to
                                                    , const QString &model, const QString &func
                                                    , const QMap<QString, QString> &para
                                                    , int *error)
{
    bool sessionRegeted=false;
    mNode.clear();
    bool sessionOk=mSession.contains(to);
    mMutex.lock();
    bool authOk=mSavedAuth.contains(to);
    mMutex.unlock();
    if(!sessionOk && !authOk)
    {
        if(error)
        {
            *error=FE_WRONG_USER_OR_PWD;
        }
        return mNode;
    }

    //step1 get sessionId
    QString sessionId;
    if(!sessionOk)
    {
        Q_ASSERT(authOk);
        mMutex.lock();
        const QPair<QString,QString> pair=mSavedAuth[to];
        mMutex.unlock();
        int itemp;
        sessionId=getSession(to,pair.first,pair.second,&itemp);
        if(sessionId.isEmpty())
        {
            //有两种可能，1.用户名密码错    2.不在线
            if(error)
            {
                *error=itemp;
            }
            return mNode;
        }
        sessionRegeted=true;

    }
    else
    {
        sessionId=mSession[to];
    }

    //step2 get data
    QDomElement elem;
    QDomDocument doc;
    elem=doc.createElement(QString("%1.%2").arg(model,func));
    elem.setAttribute("_sessionId",sessionId);

    QMap<QString, QString>::const_iterator i = para.constBegin();
    while (i != para.constEnd()) {
        elem.setAttribute(i.key(),i.value());
        ++i;
    }

    doc.appendChild(elem);
//    QString req="<%1.%2 _sessionId=\"%3\"/>";
//    req=req.arg(model,func,sessionId);

    QString req=doc.toString();

    bool needStartFinish=isSetApi(model,func);

    if(needStartFinish)sendStart(to,sessionId);
    elem=fcmlRequest(QString("netrouter@")+to,req).firstChild().toElement();
    if(needStartFinish)sendFinish(to,sessionId);

    if(0==elem.tagName().compare("error"))
    {
        int retvalue=elem.attribute("code").toInt();
        if(retvalue==FE_SESSION_NOTOK && !sessionRegeted && authOk)
        {
            const QPair<QString,QString> pair=mSavedAuth[to];
            sessionId=getSession(to,pair.first,pair.second);
            sessionRegeted=true;
            if(!sessionId.isEmpty())
            {
                elem=fcmlRequest(QString("netrouter@")+to,req).firstChild().toElement();
            }
        }
    }

    if(error)
    {
        if(0==elem.tagName().compare("error"))
        {
            *error=elem.attribute("code").toInt();
        }
        else
        {
            *error=0;
        }
    }
    return mNode;
}

void SVT_Fcml::sendStart(const QString &to, const QString &_sessionId)
{
    QDomDocument doc;
    QDomElement elem=doc.createElement("DeviceConfig.ConfigurationStarted");
    elem.setAttribute("_sessionId",_sessionId);
    elem.setAttribute("NewSessionID",_sessionId);
    doc.appendChild(elem);
    QString req=doc.toString();

    elem=fcmlRequest(QString("netrouter@")+to,req).firstChild().toElement();
}

void SVT_Fcml::sendFinish(const QString &to, const QString &_sessionId)
{
    QDomDocument doc;
    QDomElement elem=doc.createElement("DeviceConfig.ConfigurationFinished");
    elem.setAttribute("_sessionId",_sessionId);
    elem.setAttribute("NewStatus","ChangesApplied");
    doc.appendChild(elem);
    QString req=doc.toString();

    elem=fcmlRequest(QString("netrouter@")+to,req).firstChild().toElement();

}

void SVT_Fcml::endSession(const QString &to, const QString &session, int *error)
{
    QString req="<SessionManagement.endSession  sessionId=\"%1\"/>";
    req=req.arg(session);

    const QDomNode &_node=fcmlRequest(QString("netrouter@")+to,req);
    QString desp;
    int itemp=getRetValue(_node,desp);

    if(error)
    {
        *error=itemp;
    }
}

void SVT_Fcml::endAllSessions()
{
    QMap<QString,QString>::ConstIterator i=mSession.constBegin();
    while(i != mSession.constEnd())
    {
        endSession(i.key(),i.value());
        i++;
    }
    mSession.clear();
}

QString SVT_Fcml::getSession(const QString &to, const QString &usr
                   ,const QString &pwd,int *error)
{

    //test
#if 0
    QString test="<fcml from=\"router@portal\" to=\"router@cp171,router@cp190,ui7244@portal,ui7245@portal,ui7246@portal\" >"
            "<portal.router.ui7244.is type=\"ui\" active=\"false\"/></fcml>"
            ;
    QDomDocument td;
    td.setContent(test);
    QDomNode tn=td.firstChild();
    QString t;
    int itest=getRetValue(tn,t);
    //test end
#endif



    QString req="<SessionManagement.startSession username=\"%1\" password=\"%2\"/>";
    req=req.arg(usr,pwd);
    const QDomNode &_node=fcmlRequest(QString("netrouter@")+to,req);
    QString desp;
    int itemp=getRetValue(_node,desp);
    QString stemp;
    if(itemp==0)
    {
        QDomNode node=_node.firstChild();
        stemp=node.toElement().attribute("sessionId");
    }
    if(error)
    {
        *error=itemp;
    }

    if(stemp.isEmpty())
    {
        mSession.remove(to);
    }
    else
    {
#ifdef SM_ONLY_ONE_SESSION
        endAllSessions();
#endif
        mSession.insert(to,stemp);
    }
    if(error &&(*error==0)&&stemp.isEmpty())
    {
        *error=FE_SESSION_NOTOK;
    }
    return stemp;
}

void SVT_Fcml::setAuthenData(const QString &to, const QString &usr, const QString &pwd)
{
    mMutex.lock();
    mSavedAuth.insert(to,QPair<QString,QString>(usr,pwd));
    mMutex.unlock();
}

void SVT_Fcml::removeAllAuthenData()
{
    mMutex.lock();
    mSavedAuth.clear();
    mMutex.unlock();
}



bool SVT_Fcml::init(const QString &usr, const QString &pwd, int *retErrorIdx, QString *retErrorStr)
{
    endAllSessions();

    if(retErrorIdx)
    {
        *retErrorIdx=0;
    }
    if(retErrorStr)
    {
        *retErrorStr="";
    }
//    mMutex.lock();
//    mSavedAuth.clear();
//    mMutex.unlock();
    //step1 authenticate
    const char *cHead="POST /fcp/authenticate HTTP/1.1\r\n"
                      "Accept-Encoding: identity\r\n"
                      "Content-Length: %1\r\n"
                      "Host: %2\r\n"
                      "Content-Type: application/x-www-form-urlencoded\r\n"
                      "Connection: close\r\n";


    const char *cMode="<authenticate type=\"basic\""
    " username=\"%1\" password=\"%2\"/>";


    QString body=QString(cMode).arg(usr,pwd);
    QString head=QString(cHead).arg(body.size()).arg(mLinkUrl);

    SVT_Https http(mExitFlag);
    const QByteArray & array=http.rawRequest(mLinkUrl
                         ,head+"\r\n"+body);
    getCookie(array,mCookie);
    QDomDocument doc;
    QByteArray abody=SVT_Https::getBody(array);
    doc.setContent(abody,true);
    QDomNode rnode = doc.firstChild();
    if (!rnode.isElement())
        rnode = rnode.nextSibling();
    QDomElement elem = rnode.toElement();
//    qDebug()<<elem.tagName()<<elem.attribute("authenticated");
    if(0==elem.tagName().compare("authenticate"))
    {
        if(0!=elem.attribute("authenticated").compare("true",Qt::CaseInsensitive))
        {
            if(retErrorIdx)
            {
                *retErrorIdx=FE_LOGIN_WRONG_PWD;
            }
            if(retErrorStr)
            {
                *retErrorStr=elem.attribute("description");
            }
            return false;
        }
    }
    else
    {
        if(retErrorIdx)
        {
            *retErrorIdx=FE_INITPROCESS;
        }
        return false;
    }
    //step2 init


    QString sHead1="POST /fcp/init HTTP/1.1\r\n"
                 "Accept-Encoding: identity\r\n"
                 "Content-Length: %1\r\n"
                 "Host: %2\r\n"
                 "Connection: close\r\n"
                 "Content-Type: application/x-www-form-urlencoded\r\n";
    QString sBody1="<init type=\"ui\" fcmb=\"true\"/>";

    sHead1=sHead1.arg(sBody1.size()).arg(mLinkUrl);
    const QByteArray &array1=http.rawRequest(mLinkUrl
                ,appendCookie(sHead1)+"\r\n"+sBody1);
    QByteArray abody1=SVT_Https::getBody(array1);
    QDomDocument doc1;
    doc1.setContent(abody1,true);
    rnode = doc1.firstChild();
    if (!rnode.isElement())
        rnode = rnode.nextSibling();
    elem = rnode.toElement();
//    qDebug()<<elem.tagName()<<elem.hasAttribute("domain")<<elem.hasAttribute("name");
    if ( (0!=elem.tagName().compare("init"))
         ||!elem.hasAttribute("domain")
         ||!elem.hasAttribute("name")
         )
    {
        if(retErrorIdx)
        {
            *retErrorIdx=FE_INITPROCESS;
        }
        return false;
    }
    mDomain=elem.attribute("domain");
    mName=elem.attribute("name");
    return true;

}

bool SVT_Fcml::getCPList(QList<QMap<int, QString> > &retlst)
{
    retlst.clear();

    QDomNode node=fcmlRequest("router@portal","<get/>");
    node=node.firstChild();
    while(!node.isNull())
    {
        if(node.isElement())
        {
            QDomElement elem=node.toElement();
//            if(0==elem.attribute("active").compare("true",Qt::CaseInsensitive))
//            {
            QString model=elem.attribute("model");
            if(0!=model.left(3).compare("rnd",Qt::CaseInsensitive))
            {
                retlst.append(QMap<int,QString> ());
                QMap<int, QString> &map=retlst[retlst.count()-1];
                map.insert(ECV_IDENTIFIER,cpName(elem.tagName()));
                map.insert(ECV_MODEL,model);
                map.insert(ECV_SERIAL,elem.attribute("serial"));
                map.insert(ECV_NAME,elem.attribute("friendly_name"));
                if(QApplication::arguments().contains("-dbg_nocp"))
                {
                    map.insert(ECV_ONLINE,false);
                }
                else
                {
                    map.insert(ECV_ONLINE,elem.attribute("active"));
                }
            }
//            }

            node=node.nextSibling();
        }
    }
    qDebug()<<"retlst:"<<retlst;

    //step1 post request

        return !retlst.isEmpty();

}
