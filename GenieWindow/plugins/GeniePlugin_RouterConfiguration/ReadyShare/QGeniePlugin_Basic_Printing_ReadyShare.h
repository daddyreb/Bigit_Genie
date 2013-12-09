#ifndef QGENIEPLUGIN_BASIC_PRINTING_READYSHARE_H
#define QGENIEPLUGIN_BASIC_PRINTING_READYSHARE_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QFrame>
#include <QPushButton>
#include <QGridLayout>


#include "IManagerHandler.h"
#include "ILogManager.h"

#include "plugin_ui_def.h"
#include "IClickManager.h"
#include "IUiManager.h"
#include <QGroupBox>
#include <QRadioButton>
#include <QStackedLayout>

class QGeniePlugin_Basic_Printing_ReadyShare : public QWidget
{
    Q_OBJECT
public:
    explicit QGeniePlugin_Basic_Printing_ReadyShare(QWidget *parent = 0);

    void changeEvent(QEvent *e);

    void setupUi();
    void retranslateUi();
    static IManagerHandler *sHandler;
    static QStackedLayout* stackLayout;
    QLabel* lblReadyShare;
    QLabel* lblDesc;
    QGroupBox* grpSwitchType;
    QRadioButton* rdoBasic;
    QRadioButton* rdoPrinting;
    //QVBoxLayout* vlyDesc;
    QGridLayout* gridAll;
    QPushButton* btnBack;
    QPushButton* btnContinue;

    QString strReadyShare;
    QString strDesc;

    QLabel* txtBasic;
   // QLabel* txtRemote;
    QLabel* txtPrinting;

signals:
    void preIndexSend(int iPreIndex=4);
public slots:
    void BtnBackClicked( bool checked = false );
    void BtnContinueClicked( bool checked = false );
};

#endif
