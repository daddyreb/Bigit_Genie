#ifndef QGenieRSBasic_H
#define QGenieRSBasic_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QFrame>
#include <QPushButton>
#include <QGridLayout>
#include <QStackedLayout>








#include <QGroupBox>
#include <QRadioButton>
#include <QMovie>
#include <QProcess>

#include "QProgressIndicator.h"
#include "Singleton.h"

class QGenieRSBasic : public QObject,public Singleton<QGenieRSBasic>
{
    Q_OBJECT
public:
    explicit QGenieRSBasic();
	~QGenieRSBasic();
//    void changeEvent(QEvent *e);
    QProcess *mFileCheckProcess;
    QWidget * getUi();

    void retranslateUi();
    QLabel* lblReadyShare;
    QLabel* lblDevice;
    QLabel* lblDevicePash;

    QLabel* lblAlarm;
    QLabel* lblPic;

    QMovie *movie;

    QPushButton* btnBack;
    QPushButton* copyLink; //kai.yan add
    QGridLayout* gridAll;

    bool bAlarm;
    bool bFirstShow;

    QProgressIndicator* qPrg;

    void getFileExist(bool bFileExist);
    void tryStartFileCheckProcess();
    void setMovie(const QString &fname);
    void ipChanged(const QString &strIp);
protected:
    QWidget *mW;

public slots:
    void BtnBackClicked( );
//    void BtnContinueClicked( bool checked = false );

    void linkActivated ( const QString & link );
    void slot_ProcessEnd(int ret);
//    void BtnCopyLink();

};

#endif
