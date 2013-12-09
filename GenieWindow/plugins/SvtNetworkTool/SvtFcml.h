#ifndef SVTFCML_H
#define SVTFCML_H
#include <QtCore>
#include <QDomDocument>
#include "SvtNetworkTool.h"
#include "SvtFcmlError.h"

#define INVALID_PWD "Invalid Username or Password."

class SVT_NETWORKTOOL_EXPORT SVT_Fcml
{
public:
    enum E_CP_VALUE
    {
        ECV_IDENTIFIER
        ,ECV_NAME
        ,ECV_MODEL
        ,ECV_SERIAL
        ,ECV_ONLINE
        ,ECV_NUM
    };

    QString mLinkUrl;
    SVT_Fcml(bool *exitFlag,QString url);
    bool init(const QString &usr,const QString &pwd
              ,int *retErrorIdx=0,QString *retErrorStr=0);
    bool getCPList(QList<QMap<int,QString> > &retlst);
public:
    const QString &appendCookie(QString &head);
    const QDomNode &fcmlRequest(const QString &to,const QString &req);

    //0.ok      16.session not ok.    17.invalid user and pwd
    const QDomNode &fcmlReqWithRefreshSession(const QString &to,const QString &model
                                              ,const QString &func
                                              ,const QMap<QString,QString> &para
                                              ,int *error=0);
    void endSession(const QString &to,const QString &session,int *error=0);
    void endAllSessions();
    QString getSession(const QString &to,const QString &usr,const QString &pwd,int *error=0);
    void setAuthenData(const QString &to,const QString &usr,const QString &pwd);
    void removeAllAuthenData();
    void sendStart(const QString &to,const QString &_sessionId);
    void sendFinish(const QString &to,const QString &_sessionId);
protected:
    bool *mExitFlag;
    QString mUser;
    QString mPwd;
    QString mDomain;
    QString mName;
    QMap<QString ,QString> mCookie;
    QMap<QString,QString> mSession;
    QMap<QString,QPair<QString,QString> > mSavedAuth;
//    QString mCache;
    QDomNode mNode;
    QDomDocument mDoc;
    QMutex mMutex;

};

#endif // SVTFCML_H
