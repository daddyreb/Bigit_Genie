#include "WidgetTestUi.h"
#include "ui_WidgetTestUi.h"
#include "mainwindow.h"
#include "QGenieRCPageGuessAccess.h"
#include "ui_QGenieRCPageGuessAccess.h"
#include "IManagerHandler.h"
#include "GeniePluginTest.h"

WidgetTestUi::WidgetTestUi(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetTestUi)
{
    ui->setupUi(this);
}

WidgetTestUi::~WidgetTestUi()
{
    delete ui;
}

void WidgetTestUi::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void WidgetTestUi::on_but1_clicked()
{
    static int s=0;
    MainWindow *main=MainWindow::GetSingletonPtr();
    ASSERT(main);
    int tot=main->mStack->count();
    s=(1+s)%tot;
    main->mStack->setCurrentIndex(s);

    QGenieRCPageWireless *wireless=QGenieRCPageWireless::GetSingletonPtr();
    ASSERT(wireless);
    wireless->setBackgroundImg();
}

void WidgetTestUi::on_but2_clicked()
{
    static int s=0;
    QGenieRCPageWireless *wireless=QGenieRCPageWireless::GetSingletonPtr();
    ASSERT(wireless);
    int tot=wireless->mUi.stackedWidget->count();
    s=(1+s)%tot;
    wireless->mUi.stackedWidget->setCurrentIndex(s);

    wireless->setBackgroundImg();
}

void WidgetTestUi::on_but3_clicked()
{
    static int s=0;
    QGenieRCPageGuessAccess *gacc=QGenieRCPageGuessAccess::GetSingletonPtr();
    ASSERT(gacc);
    int tot=gacc->ui->stackedWidget->count();
    s=(1+s)%tot;
    gacc->ui->stackedWidget->setCurrentIndex(s);

    QGenieRCPageWireless *wireless=QGenieRCPageWireless::GetSingletonPtr();
    ASSERT(wireless);
    wireless->setBackgroundImg();

}
void WidgetTestUi::on_but4_clicked()
{
    QGenieRCPageGuessAccess *gacc=QGenieRCPageGuessAccess::GetSingletonPtr();
    ASSERT(gacc);
    static bool sb=true;
    sb=!sb;
    gacc->setDisabled(sb);
}
void WidgetTestUi::on_but5_clicked()
{
	static int stemp=0;
	stemp=(stemp+1)%6;
	IUiManager *uimanager=MainWindow::sHandler->getUiManager();
	QString ico=QString("Wireless%1.png").arg(stemp);
	uimanager->setBigButtonIcon(ico,UUID_OF_THIS);

}

void WidgetTestUi::on_but6_clicked()
{
    static int stemp=0;
    stemp=(stemp+1)%3;
    IUiManager *uimanager=MainWindow::sHandler->getUiManager();

    GeniePlugin_RouterConfiguration *pi=GeniePlugin_RouterConfiguration::GetSingletonPtr();
    uimanager->setBigButtonSubState(pi,(ENUM_BIGBT_SUB_STATE)stemp);
}

void WidgetTestUi::on_but7_clicked()
{
   IUiManager *uimanager=MainWindow::sHandler->getUiManager();
   uimanager->command(UI_CMD_DIALOG,"ROUTERCONFIG");
}
