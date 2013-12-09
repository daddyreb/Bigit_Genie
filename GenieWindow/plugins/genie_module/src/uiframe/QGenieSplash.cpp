#include "stable.h"


QGenieSplash::QGenieSplash(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QGenieSplash)
{
    ui->setupUi(this);
    setWindowFlags(Qt::SplashScreen/*|Qt::WindowStaysOnTopHint*/);
//    this->setAttribute(Qt::WA_TranslucentBackground);
    ui->progresssplash->hide();
}

QGenieSplash::~QGenieSplash()
{
    delete ui;
}

void QGenieSplash::changeEvent(QEvent *e)
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
