#include "TestForm.h"
#include "ui_TestForm.h"
#include "GeniePluginTest.h"

TestForm::TestForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TestForm)
{
    ui->setupUi(this);
}

TestForm::~TestForm()
{
	qDebug("TestForm::~TestForm()");
    delete ui;
}

void TestForm::changeEvent(QEvent *e)
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

void TestForm::on_but_1_clicked()
{
    qDebug("TestForm::on_but_1_clicked()");
    IUiManager *uimanager=TestForm::sHandler->getUiManager();
    IPluginInf *pi=GeniePlugin_NetWorkProblem::sSinglePtr;
     Q_ASSERT(uimanager && pi);
     uimanager->setWaitState(pi,"");
}

void TestForm::on_but_2_clicked()
{
    qDebug("TestForm::on_but_2_clicked()");
    IUiManager *uimanager=TestForm::sHandler->getUiManager();
    IPluginInf *pi=GeniePlugin_NetWorkProblem::sSinglePtr;
     Q_ASSERT(uimanager && pi);
     uimanager->restorNormalState(pi,"");
}
