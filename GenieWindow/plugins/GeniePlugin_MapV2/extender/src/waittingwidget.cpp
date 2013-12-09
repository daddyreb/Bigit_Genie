#include "waittingwidget.h"
#include "extendermainwindow.h"
#include "language_mainwindow.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QEvent>
#include <QtWidgets/QLabel>

#define INDICATOR_SIZE QSize(70,70)
#define INDICATOR_ANIMATION_DELAY 100

WaittingWidget::WaittingWidget(QWidget *parent) :
    QWidget(parent)
{
    Indicator = new MyProgressIndicator(this);
    Indicator->setFixedSize(INDICATOR_SIZE);
    Indicator->setAnimationDelay(INDICATOR_ANIMATION_DELAY);

    PromptLabel = new QLabel(this);

    QVBoxLayout *verLayout = new QVBoxLayout();
    verLayout->addWidget(Indicator);
    verLayout->addWidget(PromptLabel);
    verLayout->setAlignment(Indicator,Qt::AlignCenter);
    verLayout->setAlignment(PromptLabel,Qt::AlignCenter);


    QHBoxLayout *layOut = new QHBoxLayout(this);
    layOut->addLayout(verLayout);
    layOut->setAlignment(verLayout,Qt::AlignCenter);

    retranslateUi();
}

WaittingWidget::~WaittingWidget()
{
    stop();
}

void WaittingWidget::changeEvent(QEvent *event)
{
    if(event->type() == QEvent::LanguageChange)
    {
        retranslateUi();
    }

    QWidget::changeEvent(event);
}

void WaittingWidget::retranslateUi()
{
    PromptLabel->setText(translateText(L_WAIT_LBL, QString("Waiting...")));
}

QString WaittingWidget::translateText(int idx, const QString &defaultText)
{
    QString text;
    emit translateText(idx,&text);
    return text.isEmpty() ? defaultText : text;
}

void WaittingWidget::start()
{
    if(!Indicator->isAnimated())
    {
        Indicator->startAnimation();
    }
}

void WaittingWidget::stop()
{
    if(Indicator->isAnimated())
    {
        Indicator->stopAnimation();
    }
}

