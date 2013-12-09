#ifndef QGENIEPLUGINPROXYMANAGER_H
#define QGENIEPLUGINPROXYMANAGER_H
#ifndef IN_STABLE_H
#include <QObject>
#include "Singleton.h"
#include "IPluginInf.h"
#include <QUuid>
#include "QGeniePluginInfoManager.h"
#include <QAbstractItemModel>
#endif

class QGeniePluginInfoManager;
class QGeniePagePluginProxy;
class QGenieFrameStackView;
class IPluginInf;
class QGenieButtonBig;
class QGeniePluginParsor;
class plug_inf;
class QGenieButtonStackViewController;

class QGeniePluginProxyManager:public QObject ,public Singleton<QGeniePluginProxyManager>
{
    Q_OBJECT
public:
    QGeniePluginProxyManager();
    ~QGeniePluginProxyManager();
    QGeniePagePluginProxy *createPage(IPluginInf *pi);
    void setStackView(QGenieFrameStackView *stackview);
    IPluginInf *getPluginInf(QGeniePagePluginProxy *w );

    QGeniePagePluginProxy *getPagePluginProxy(IPluginInf *pi);
    void startupAllPi();
 const   plug_inf *getPluginInf(QGenieButtonBig *bgbt);
plug_inf *getPluginInf(QUuid uuid);
plug_inf *getPluginInf(IPluginInf *pi);
 const plug_inf *getPluginInfFromLeftBut(QGenieButtonStackViewController * but);

    bool mLoadOk;
    void detectWidgets();

  //  void registNoneWndPlugin(IPluginInf *pi);
  //  void startupAllNoneWndPlugins();
    void addAllPluginToUi();
  //  IPluginInf *mPluginDiagnose;

    IPluginInf *createPluginInfFromDll(QString filename);
#ifdef CAL_VERSION
    QString calVersionStr() const;
#endif
    void unloadPi(QString filename);
    void loadPi(QString filename);



protected:
 //   void registPluginWidget(QGeniePagePluginProxy *w,IPluginInf *pi);
    bool isValitPluginFileName(QString s) const;
 //   QHash<QGeniePagePluginProxy *, IPluginInf *> mProxyHash;
 //   QHash<QPluginLoader *,IPluginInf *> mLoaderHash;
 //   QMap<int,QGeniePagePluginProxy *> mStackIdxMap;
 //   QPluginLoader *mCurrentLoader;
#ifdef CAL_VERSION
    int mCalVersion;
#endif

    QGenieFrameStackView * mStackView;
//    int mCurrentStackIdx;
//    QSet<IPluginInf *> mNoneWndPlugins;
//    QTimeLine *mTimeLineForNoneWndPlugin;

//    QMap<int,IPluginInf *> mPiCache;
//    QGeniePluginParsor *mParsor;
    QGeniePluginInfoManager mInfoManager;
protected slots:
 //   void slot_StackIdxChanged(int idx);
    void slot_StartupAllNoneWndPlugins();
	friend class QGenieFrameDebug;


};

#endif // QGENIEPLUGINPROXYMANAGER_H
