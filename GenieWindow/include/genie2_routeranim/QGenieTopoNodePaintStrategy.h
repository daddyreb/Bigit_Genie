#ifndef QGENIETOPONODEPAINTSTRATEGY_H___
#define QGENIETOPONODEPAINTSTRATEGY_H___
#include <QList>
#include <QMap>
#include <QPainter>
#include <QWidget>
#include <QUuid>
#include <QMutex>

//class IInfoManager;
//class ILogManager;

#define ANIMATION_INTERVAL 500

enum ANIM_TYPE{
    AT_BIG1=0,
    AT_BIG3,
    AT_MAP,
    AT_NUM,
    AT_INVALID
};

enum IMAGE_ENUM
{
    IE_B11
    ,IE_B12
    ,IE_B31
    ,IE_B32
    ,IE_NORMAL1
    ,IE_NORMAL2
    ,IE_SELECTED1
    ,IE_SELECTED2
    ,IE_OFFLINE
    ,IE_NUM
};
class QGenieTopoNodePaintStrategy;

class QGenieRouteWidget:public QWidget
{
public:
    QGenieRouteWidget(QWidget *parent=NULL);
    ~QGenieRouteWidget();
    void reloadImage();
//    void setInfoManager(IInfoManager *infomanager);
//    void setLogManager(ILogManager *logmanager,QUuid uuid);
protected:
    QGenieTopoNodePaintStrategy *mPaintStrategy;
    void timerEvent(QTimerEvent *);
    void paintEvent(QPaintEvent * /*event*/);
    ANIM_TYPE mType;
//    IInfoManager *mInfoManager;
//    ILogManager *mLogManager;
    QUuid mUuidForLog;
//    QMutex mMutex;
};


class QGenieRouteWidget_Map:public QGenieRouteWidget
{
public:
    QGenieRouteWidget_Map(QWidget *parent=NULL);
};

class QGenieRouteWidget_B3:public QGenieRouteWidget
{
public:
    QGenieRouteWidget_B3(QWidget *parent=NULL);
};

class QImageNameGengerator
{
public:
    QImageNameGengerator(const QString &s=QString());
    QString getImageName(int type);
    QString getRouterName();
protected:
//    IInfoManager *mInfoManager;
    QString mRouterName;
};

class QGenieTopoNodePaintStrategy
{
public:
//    QGenieTopoNodePaintStrategy(ANIM_TYPE type,IInfoManager *infomanager,QWidget *linkw=NULL);
    QGenieTopoNodePaintStrategy(ANIM_TYPE type,QWidget *linkw=NULL,const QString &model=QString());

    ~QGenieTopoNodePaintStrategy();
    void paint(QPainter *painter,bool mouseover,bool online);
    void paint_Map(QPainter *painter,bool mouseover,bool online);
    void paint_B1_B3(QPainter *painter,bool b1);
    const QSize &getSize();
    const QString &getImageName();
//    void paint_B3(QPainter *painter,bool mouseover,bool online);
protected:
    void init(QImageNameGengerator *image_gener);
    QStringList mImgs;
    int mMovieParaIdx;
    ANIM_TYPE mType;
    QWidget *mLinkW;
    QSize mSize;
    QString mImageName;


};



#endif
