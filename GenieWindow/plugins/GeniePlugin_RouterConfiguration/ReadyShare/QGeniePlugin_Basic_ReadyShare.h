#ifndef QGENIEPLUGIN_BASIC_READYSHARE_H
#define QGENIEPLUGIN_BASIC_READYSHARE_H

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
#include "QProgressIndicator.h"

class QGeniePlugin_Basic_ReadyShare : public QWidget
{
    Q_OBJECT
public:
    explicit QGeniePlugin_Basic_ReadyShare(QWidget *parent = 0);

    int intPreIndex;
    void changeEvent(QEvent *e);

    void setupUi();
    void retranslateUi();
    static IManagerHandler *sHandler;
    static QStackedLayout* stackLayout;

    QLabel* lblReadyShare;
    QLabel* lblDesc;
    QGroupBox* grpSwitchType;
    QRadioButton* rdoBasic;

    //QVBoxLayout* vlyDesc;
    QGridLayout* gridAll;
    QPushButton* btnBack;
    QPushButton* btnContinue;

    QString strReadyShare;
    QString strDesc;

    QLabel* txtBasic;
    //QLabel* txtRemote;
    //QLabel* txtPrinting;
signals:
    void preIndexSend(int iPreIndex=4);
public slots:
    void BtnBackClicked( bool checked = false );
    void BtnContinueClicked( bool checked = false );
};

#endif // QGENIEPLUGIN_NOT_SURPORT_READYSHARE_H
