#include "GenieAboutForm.h"
#include "ui_GenieAboutForm.h"

IManagerHandler *GenieAboutForm::mHandler=NULL;

GenieAboutForm::GenieAboutForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GenieAboutForm)
{
    ui->setupUi(this);
}

GenieAboutForm::~GenieAboutForm()
{
    delete ui;
}

void GenieAboutForm::changeEvent(QEvent *e)
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
