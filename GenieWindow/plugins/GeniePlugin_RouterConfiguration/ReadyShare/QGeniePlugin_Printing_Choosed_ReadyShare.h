#ifndef QGENIEPLUGIN_PRINTING_CHOOSED_READYSHARE_H
#define QGENIEPLUGIN_PRINTING_CHOOSED_READYSHARE_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QFrame>
#include <QPushButton>
#include <QGridLayout>
#include <QStackedLayout>
#include <QLineEdit>


#include "IManagerHandler.h"
#include "ILogManager.h"

#include "plugin_ui_def.h"
#include "IClickManager.h"
#include "IUiManager.h"
#include <QGroupBox>
#include <QRadioButton>

class QGeniePlugin_Printing_Choosed_ReadyShare : public QWidget
{
    Q_OBJECT
public:
    explicit QGeniePlugin_Printing_Choosed_ReadyShare(QWidget *parent = 0);

    void changeEvent(QEvent *e);

    void setupUi();
    void retranslateUi();
    static IManagerHandler *sHandler;
    static QStackedLayout* stackLayout;

    QLabel* lblReadyShare;

    QLabel* lblDesc;
    QLabel* lblHow;
    QLabel* lblStep1;
    QLabel* lblStep2;
    QLabel* lblStep3;
    QLabel* lblStep4;
    QLabel* lblStart;
    QLabel* lblUser;
    QLabel* lblPwd;

    QLineEdit* txtUser;
    QLineEdit* txtPwd;


   // QLabel* lblDevice;
    //QLabel* lblDevicePash;
    QGroupBox* grpSwitchType;
    QRadioButton* rdoBasic;
    QRadioButton* rdoRemote;
    //QVBoxLayout* vlyDesc;
    QGridLayout* gridAll;
    QPushButton* btnBack;
    QPushButton* btnRegister;

    QString strReadyShare;
    QString strDesc;
signals:
    void ShowPreForm();
public slots:
    void BtnBackClicked( bool checked = false );
    void BtnContinueClicked( bool checked = false );
};

#endif
