#ifndef QGenieNetgearInterface_H
#define QGenieNetgearInterface_H

#include <QtCore>
#include "INetgearInterface.h"
//class QGenieNetgearInterface_todelete;
class QGenieNetgearInterface;
class SVT_Fcml;
class _SoapNode{
public:
    enum _Flags{
        F_INPROCESS=(1<<0)
    };

    _SoapNode(QGenieNetgearInterface *);
    ~_SoapNode();
    int cmd;
    QStringList paralist;
    QObject *receiver;
    const char * slot;
    int session;
    QStringList retlist;
    quint32 flag;
//    const void *otherData;
//    const void *otherData2;
    QGenieNetgearInterface *mManager;
};

//class INetgearInterface
//{
//public:


//    INetgearInterface(){};
//    virtual ~INetgearInterface(){};
//    virtual int netgearCommand( _SoapNode *)=0;
//};

enum NETGEAR_INTERFACE_FLAGS
{
    NIF_IN_DEBUG_MODE=(1<<0)
};


class QGenieNetgearInterface : public QObject,public INetgearInterface
{
    Q_OBJECT
public:
    enum _ThreadType
    {
        TT_LOCAL
        ,TT_REMOTE
        ,TT_NUM
    };


    explicit QGenieNetgearInterface(QObject *parent , bool dbg_mode);
    ~QGenieNetgearInterface();
    int netgearCommand(IFACE_COMMAND cmd,const QStringList &cmd_paras,QObject *receiver,const char *slot);
//    int netgearCommand(_SoapNode *node);
    void setConfiguration(IFACE_SETTINGS cmd, const QStringList &seting_paras);
    void doReturn(_SoapNode *node);

signals:
    void signal_CommandReturn(int session,const QStringList &cmdlist);
public slots:
    void slot_ReceiverDeleted(QObject *obj);
    void slot_ThreadDataReturned(_SoapNode *node, QStringList lst);
public:
//    QGenieNetgearInterface_todelete *mIface;
    QList<_SoapNode *> mNodeList;
//    friend class QGenieNetgearInterface_todelete;
    SVT_Fcml *mFcml;
    QMutex mMutex;
    bool mExitFlag;
    quint32 mInterfaceFlags;


protected:
     QVector<IThreadInterface *> mThreads;
};

#endif // QGenieNetgearInterface_H
