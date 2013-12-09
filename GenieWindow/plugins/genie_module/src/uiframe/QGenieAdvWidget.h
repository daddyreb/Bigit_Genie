#ifndef QGENIE_ADVWIDGET_H
#define QGENIE_ADVWIDGET_H
#ifndef IN_STABLE_H
#include <QObject>
#include <QLabel>
#include <Singleton.h>
#include <QHttp>
#include <QMap>
#include <QFile>
#include <QUrl>
#endif

enum ADV_STATE
{
    AS_INIT,
    AS_GETING_TOTAL_DSP,
    AS_TOTAL_DSP_OK,
    AS_GETTING_PIC,
    AS_PIC_OK,
    AS_TOTAL_ERROR,
    AS_PIC_ERROR
};


class QGenieAdvWidget : public QLabel,public Singleton<QGenieAdvWidget>
{
    Q_OBJECT

public:
    QGenieAdvWidget(QWidget *parent = 0);
    ~QGenieAdvWidget();
    QHttp *totalhttp;
    QHttp *http;
    QHttp *httpPic;
    QFile *file;
    //QUrl qurlSettingTxt;
    QUrl url;
    void SetSettingTxtUrl(QString strUrl,int port=80);
    void SetTotalDiscriptionUrl(QString strUrl,int port=80);

    void startup(QString route="");

    QMap<QString,QString> mRoutMapUrl;

    QMap<QString,QString> qmapPicUrl;
    QMap<QString,QString> qmapLinkUrl;
    QMap<QString,QString> qmapFileMd5;
    QMap<QString ,int> qmapIntval;

    ADV_STATE mState;

    QString mRouteCache;
   // QString mCurDispName;
    QTimer* qtmAdvChange;
    QList<QFile *> mFileList;
    QMovie *mGifMovie;

    bool httpRequestAborted;
    int httpGetId;
    bool donwloadFile(QString strFileName);
    int mPicCount;
    int iPicCoutLoop;
    QString strCurrentUrl;
    QString strCurrentFileName;
    QString strCurrentAdvDir;
    void	mouseReleaseEvent ( QMouseEvent * event );
    bool  md5_verify(QString strFileName);
    bool deleteDirectory(QString DirName,int stack=0);
    void getAdvWhenRouteChanged(QString route);
    void startGetAdvForRoute(QString route);
protected:
    void mousePressEvent ( QMouseEvent * event )  ;
    bool parseTotalDiscription(const QString &str);
    bool parseRouteDiscription(const QString &para,
                               QMap<QString,QString> &picurlmap,
                               QMap<QString,QString> &linkurlmap,
                               QMap<QString,QString> &md5map,
                               QMap<QString,int> &intervalmap);
protected slots:
    void slot_AdvTimeout ();

    void slot_InitialMap(bool  error);
    void slot_DownloadPicFinished(bool  error);
    void slot_TotalDiscriptionDownloaded(bool error);

};

#endif // QGenieAdvWidget_H
