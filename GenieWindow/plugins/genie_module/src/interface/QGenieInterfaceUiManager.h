#ifndef QGENIEINTERFACEUIMANAGER_H
#define QGENIEINTERFACEUIMANAGER_H

class IPluginInf;

class QGenieInterfaceUiManager:public IUiManager,public Singleton<QGenieInterfaceUiManager>
{
public:
        QGenieInterfaceUiManager();
        virtual void setWaitState(IPluginInf *pi,QString resion);
        virtual void restorNormalState(IPluginInf *pi,QString resion);
        virtual void changeStackIdx(int idx);
        virtual int command(QString cmd,QString para);
//        virtual void setBigButtonState(IPluginInf *pi,ENUM_PLUGIN_STATUS state,int idx=0);
        virtual void setBigButtonSubState(IPluginInf *pi,ENUM_BIGBT_SUB_STATE state,int idx=0);
     //   virtual QClipboard *getClipBoard();
        virtual QString getClipBoardString();
        virtual void setClipBoardString(const QString s);

        virtual void setBigButtonIcon(const QString &ico,QUuid uuid,int idx=0);
        virtual void setBigButtonUserStateText(const QString &stylesheet,const QUuid &uuid,int idx=0);
        virtual void setBigButtonUserStateBKGround(const QString &stylesheet,const QUuid &uuid,int idx=0);
        virtual void setNetStateFlag(QUuid uuid,bool setit=true);
        virtual void setRouteTypeFlag(QUuid uuid,bool setit=true);
        virtual void setGatewayFlag(QUuid uuid,bool setit=true);
        virtual void setBackGroundImg(QUuid uuid,QString img,QString para="");
#ifdef _WIN32
        virtual bool checkAdmin();
#endif

        virtual void setLeftButtonChecked(IPluginInf *pi,int idx = 0);

        QSet<QUuid> mNetStateUuids;
        QSet<QUuid> mRouteTypeUuids;
        QSet<QUuid> mGatewayUuids;
        NET_STATE mCurState;
        QString mRouteType;
        inline void setCurNetState(NET_STATE st)
        {
            mCurState=st;
        };

        void notifyNetState(int st);
        void notifyRouteType(QString type);
        void notifyGateway(QString str);
        void turnToThisPage(QUuid uuid,int msg);
        void setRouteType(const QString &rts);
        inline const QString &routeType()
        {
            return mRouteType;
        }
        void setWifiName(const QString &wname);
};

#endif
