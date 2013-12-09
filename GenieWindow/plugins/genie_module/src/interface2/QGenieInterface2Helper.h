#ifndef QGENIEINTERFACE2HELPER_H
#define QGENIEINTERFACE2HELPER_H
#include "Singleton.h"
#include <QtCore>
#include <QUuid>
class _PluginInfoHelper;
class QGenieSingleProcessHandler;
#include <QVariant>
class QGenieInterface2LogManager;
class IPluginInf2;


int indexOfUuid(const QUuid &uuid);
QUuid uuidOfIndex(int index);
class QGenieInterface2Helper:public QObject,public Singleton<QGenieInterface2Helper>
{
    Q_OBJECT
public:
    QGenieInterface2Helper();
    ~QGenieInterface2Helper();
    void loadAllPlugins();
    void registerUi(const QVariant &para);
    void registerDBGUi(const QVariant &para);
    QVariant tellPluginMessage(const QVariant &para);
    QVariant tellPluginMessage(int uuid,int message,const QVariant &paras=QVariant());
    void tellAllPlutinMessage(int cmd,const QVariant &paras=QVariant());
    QVariant getImageName(const QVariant &paras);
    QString getPluginFileName(int uuid);
    int loadPlugin(const QString &fname); //return uuid index
    void initDBGModel(const QVariant &paras);
    bool mbConflictWithOtherProcess;

protected:
    _PluginInfoHelper *mInfoHelper;
    QGenieSingleProcessHandler *mSingleProcessHandler;
#ifdef USE_LAZYLOAD
    QSet<int> mCompletePis;
    QTimer mLazyTimer;
    int mCurLazyPi;
protected slots:
    void slot_LazyLoad();
#endif

protected slots:
    void slot_otherSameProcess();
};

#endif // QGENIEINTERFACE2HELPER_H
