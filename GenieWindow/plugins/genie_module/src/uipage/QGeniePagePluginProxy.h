#ifndef QGENIEPAGEPLUGINPROXY_H
#define QGENIEPAGEPLUGINPROXY_H


class IPluginInf;
class QGenieButtonBig;

enum PLUGIN_STATE
{
    PLST_GOOD,
    PLST_WARRING,
    PLST_ERROR,
    PLST_WAITING,
    PLST_NUM
};


class QGeniePagePluginProxy:public QWidget
{
 //   Q_OBJECT
public:
    QGeniePagePluginProxy(QWidget *parent=NULL);
    void setPluginWidget(QWidget *w,IPluginInf *pi);
    void setBigButton(QGenieButtonBig *but);
 //   void setIdx(int i);
    inline void setState(PLUGIN_STATE st)
    {
        mState=st;
    };

protected:
    Ui::QGeniePagePluginProxy mUi;
    IPluginInf *mPluginInf;
    PLUGIN_STATE mState;
//    QGenieButtonBig *mRelatedBigButton;
 //   int mIdx;
protected:
    void paintEvent(QPaintEvent *event);
    friend class QGenieStackManager;
};

#endif // QGENIEPAGEPLUGINPROXY_H
