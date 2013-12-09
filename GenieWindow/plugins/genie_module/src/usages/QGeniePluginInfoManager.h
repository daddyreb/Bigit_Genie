#ifndef QGENIEPLUGININFOMANAGER_H
#define QGENIEPLUGININFOMANAGER_H


class IPluginInf;
class QGeniePluginProxyManager;
class QGeniePagePluginProxy;
class QGenieButtonStackViewController;
class QGenieButtonBig;
class QPluginLoader;
class QAbstractItemModel;

enum PLUGIN_MODULE_STATE
{
    PMS_INIT,
    PMS_LOADED,
    PMS_WND_CREATED,
    PMS_UNLOADED,
    PMS_STARTED,
    PMS_ERROR
};

enum PLUGIN_FLAG
{
    START_MINI=1<<0
};


class plug_inf
{
public:
    plug_inf();
    QPluginLoader *loader;
    QString filename;
    QString version;
    QUuid uuid;
    PLUGIN_MODULE_STATE state;
    IPluginInf *pi;
    QGeniePagePluginProxy *page;
    QList<QGenieButtonBig *> butbig;
#ifdef ENABLE_MULTI_LEFT_BUTTON
    QList<QGenieButtonStackViewController *> leftpanbut;
#else
    QGenieButtonStackViewController * leftpanbut;
#endif
    QList<QAction *> actions;
    QMap<int,QString> lanTexts;
    bool havewnd;
    int flag;
//    QMap<int,int> stateCache;
    QMap<int,int> substateCache;
    QMap<int,QString> styleCache;
    QMap<int,QString> bkgCache;
//    friend class QGeniePluginInfoManager;
//    friend class QGeniePluginProxyManager;
//    friend class QGenieBig

};

class QGeniePluginInfoManager
{
    QGeniePluginInfoManager();
    ~QGeniePluginInfoManager();
    void createLineFromFileName(QString filename);
    plug_inf *getLineFromFileName(QString filename);
    QList<plug_inf *> mInfList;
    QMap<QString,plug_inf *> mFileNameIdx;

    QMap<IPluginInf *,plug_inf *> mPiIdx;
    QMap<QGeniePagePluginProxy *,plug_inf *> mPageIdx;
    QMap<QGenieButtonStackViewController *,plug_inf *> mLeftButtonIdx;
    QMap<QGenieButtonBig *,plug_inf *> mBigButtonIdx;
    QMap<QUuid,plug_inf *> mUuidIdx;

    void updateIdxs();
    void clearAllCaches();
    void synToModel(QAbstractItemModel *model);


    friend class QGeniePluginProxyManager;
	friend class QGenieFrameDebug;
};

#endif // QGENIEPLUGININFOMANAGER_H
