#ifndef QGENIEPLUGIN_BASIC_CHOOSED_READYSHARE_H
#define QGENIEPLUGIN_BASIC_CHOOSED_READYSHARE_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QFrame>
#include <QPushButton>
#include <QGridLayout>
#include <QStackedLayout>


#include "IManagerHandler.h"
#include "ILogManager.h"

#include "plugin_ui_def.h"
#include "IClickManager.h"
#include "IUiManager.h"
#include <QGroupBox>
#include <QRadioButton>

class QGeniePlugin_Basic_Choosed_ReadyShare : public QWidget
{
    Q_OBJECT
public:
    explicit QGeniePlugin_Basic_Choosed_ReadyShare(QWidget *parent = 0);

    void changeEvent(QEvent *e);


    void setupUi();
    void retranslateUi();
    static IManagerHandler *sHandler;
    static QStackedLayout* stackLayout;

    QLabel* lblReadyShare;
    QLabel* lblDesc;

    QLabel* lblDevice;
    QLabel* lblDevicePash;
    QGroupBox* grpSwitchType;
    QRadioButton* rdoBasic;
    QRadioButton* rdoRemote;

    //QVBoxLayout* vlyDesc;
    QPushButton* btnBack;
    QPushButton* btnContinue;
    QGridLayout* gridAll;


    QString strReadyShare;
    QString strDesc;
    QString strRouterIp;

    QString strUrl;

signals:
    void ShowPreForm();
public slots:
    void BtnBackClicked( bool checked = false );
    void BtnContinueClicked( bool checked = false );
    void ipChanged(QString strIp);
    void linkActivated ( const QString & link );
};

#endif
