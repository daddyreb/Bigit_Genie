#ifndef QGenieUpdateObject_H
#define QGenieUpdateObject_H

#include <QObject>
#include <QtGui>
#include <QtWidgets>

class SVT_HttpDownload;
class SVT_HttpDownloadPool;
class QGenieUpdateObject : public QObject
{
    Q_OBJECT
public:
    explicit QGenieUpdateObject(QObject *parent = 0);
    ~QGenieUpdateObject();
    void openDialog();
protected:
    void doPageFunc();
    void goToFinishPage(int lan0,int lan1=-1);
    void downLoadFile(const QString &url
                      ,quint16 port
                      ,const QString &local_file
                      ,quint64 startpos=0);
    void translateUi();
    void setLeftButLan();
signals:

public slots:
    void slot_HttpDone(bool error,const QString& serror);
    void slot_HttpProcess(quint64 done,quint64 total,quint32 speed);
//    void slot_HttpDestroyed(QObject *obj);
    void slot_ButClicked();
    void slot_StartProcess();
protected:
    QDialog *mDialog;
    QStackedWidget *mStack;
    SVT_HttpDownload *mHttp;
    SVT_HttpDownloadPool *mPool;
    QLabel *lab_remain_r;
    QString mLocalDir;
    QPushButton *but_left;
    QPushButton *but_right;
    QProgressBar *mProgressBar;
    QSettings *mSettings;
    QTimer mTimer;
    int mRefreshTimes;

};

#endif // QGenieUpdateObject_H
