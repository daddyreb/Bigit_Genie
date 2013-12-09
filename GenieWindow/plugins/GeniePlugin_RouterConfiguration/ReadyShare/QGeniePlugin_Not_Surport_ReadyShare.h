#ifndef QGENIEPLUGIN_NOT_SURPORT_READYSHARE_H
#define QGENIEPLUGIN_NOT_SURPORT_READYSHARE_H

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


#include <QHttpRequestHeader>
#include <QByteArray>
#include <QDebug>
#include <QHttp>
#include <QAuthenticator>

#include "QProgressIndicator.h"

#include <QTimer>

class QGeniePlugin_Not_Surport_ReadyShare : public QWidget
{
    Q_OBJECT
public:
    explicit QGeniePlugin_Not_Surport_ReadyShare(QWidget *parent = 0);

    void changeEvent(QEvent *e);

    void setupUi();
    void retranslateUi();
    static IManagerHandler *sHandler;
    static QStackedLayout* stackLayout;

    QLabel* lblErrorString;

    QLabel* lblReadyShare;
    QLabel* lblDesc;
    QGroupBox* grpSwitchType;
    QRadioButton* rdoBasic;
    QRadioButton* rdoRemote;
    QRadioButton* rdoPrinting;
    //QVBoxLayout* vlyDesc;
    QGridLayout* gridAll;
    QPushButton* btnBack;
    QPushButton* btnContinue;

    QProgressIndicator* qPrg;


    QLabel* lblResult;

   // QTimer* timerHttp;


    QString strReadyShare;
    QString strDesc;

    QLabel* txtBasic;
    QLabel* txtRemote;
    QLabel* txtPrinting;

signals:

public slots:
    void BtnBackClicked( bool checked = false );
};

#endif // QGENIEPLUGIN_NOT_SURPORT_READYSHARE_H
