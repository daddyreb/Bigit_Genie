#ifndef QGeniePluginParsor_H
#define QGeniePluginParsor_H

class IPluginInf;


struct PlgMsg{
    QUuid uid;
    bool bind;
    bool haveW;
    QString versionStr;
};

class QGeniePluginParsor:public QObject
{
public:
    QGeniePluginParsor(QObject *parent=NULL);
    bool parsePlugin(IPluginInf* pi, PlgMsg *msg);
    QString mResult;
    void clearResult();
    void clearUidCache();
protected:
    bool parseUuid(QString s,QUuid *uuid);
    bool parseBind(QString s,bool *bind);
    bool parseHaveWnd(QString s,bool *haveWnd);
    bool parseVersion(QString s,QString *version);
    bool checkBlackList(QUuid uuid);
    QList<QUuid> mUidList;
    QSet<QUuid> mBlackList;
    IPluginInf *mPi;
};

#endif // QGeniePluginParsor_H
