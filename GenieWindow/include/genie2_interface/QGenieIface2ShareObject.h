#ifndef QGENIEIFACE2SHAREWIDGET_H
#define QGENIEIFACE2SHAREWIDGET_H

#include <QWidget>
class IHandlerInf;
class INetgearInterface;
class QGenieRouteWidget;

enum SHARED_OBJECT_FLAGS
{
    SOF_EXIT_DIALOG_ON=(1<<0)
};

class QGenieIface2ShareObject : public QObject
{
    Q_OBJECT
public:
    explicit QGenieIface2ShareObject();
    ~QGenieIface2ShareObject();
    static QGenieIface2ShareObject *sSingleInstance;
    static int sUuidIdx;
    IHandlerInf *mHandler;
    QString getDbgResource(const QString &s);
    INetgearInterface *mNetgearInfoM;
    QList<QGenieRouteWidget *> mRouterWidgets;
    QString mDBGRouteModel;
    QWidget *mMainFrame;
    quint32 mFlags;
protected:
    QString mReSourcePrefix;

};

#endif // QGENIEIFACE2SHAREWIDGET_H
