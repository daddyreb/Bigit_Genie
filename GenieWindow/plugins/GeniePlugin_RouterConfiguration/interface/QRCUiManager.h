#ifndef QRCUIMANAGER_H
#define QRCUIMANAGER_H
#include "IUiManager.h"

class QRCUiManager : public IUiManager
{
public:
    QRCUiManager();
    virtual void setWaitState(IPluginInf *pi,QString resion);
    virtual void restorNormalState(IPluginInf *pi,QString resion);
    virtual void changeStackIdx(int idx);
    virtual int command(QString cmd,QString para);

   // virtual QClipboard *getClipBoard();
    virtual QString getClipBoardString();
    virtual void setClipBoardString(const QString s);

    virtual void setBigButtonUserStateUi(QString stylesheet,QUuid uuid,int idx);

    /*
      idx means the idx of bigbutton ,if the plugin have more than one bigbutton
    */
	virtual void setBigButtonIcon(const QString &ico,QUuid uuid,int idx=0);
    virtual void setBigButtonState(IPluginInf *pi,ENUM_PLUGIN_STATUS state,int idx);
	void setBigButtonSubState(IPluginInf *pi,ENUM_BIGBT_SUB_STATE state,int idx);

    virtual void setNetStateFlag(QUuid uuid,bool setit=true);
    virtual void setRouteTypeFlag(QUuid uuid,bool setit=true);
    virtual void setGatewayFlag(QUuid uuid, bool setit);

    virtual void setBackGroundImg(QUuid uuid,QString img,QString para="");
    inline void setBase(IUiManager *base)
    {
        mBase=base;
    }
    bool mInprocess;
protected:
    IUiManager *mBase;
};

#endif // QRCUIMANAGER_H
