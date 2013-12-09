#include "waitpanel.h"
#include "QProgressIndicator.h"
#include "language_Map.h"
#include "PowerlineDefine.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QEvent>
#include <QLabel>
#include <QDebug>


#define INDICATOR_SIZE QSize(70,70)
#define INDICATOR_ANIMATION_DELAY 100

WaitPanel::WaitPanel(QWidget *parent) :
    QWidget(parent)
{
    m_indicator = new QProgressIndicator(this);
    m_indicator->setFixedSize(INDICATOR_SIZE);
    m_indicator->setAnimationDelay(INDICATOR_ANIMATION_DELAY);

    m_promptLabel = new QLabel(this);

    QVBoxLayout *verLayout = new QVBoxLayout();
    verLayout->addWidget(m_indicator);
    verLayout->addWidget(m_promptLabel);
    verLayout->setAlignment(m_indicator,Qt::AlignCenter);
    verLayout->setAlignment(m_promptLabel,Qt::AlignCenter);


    QHBoxLayout *layOut = new QHBoxLayout(this);
    layOut->addLayout(verLayout);
    layOut->setAlignment(verLayout,Qt::AlignCenter);

    retranslateUi();
}

WaitPanel::~WaitPanel()
{
    stop();

	qDebug() << "WaittingPanel exit";
}

void WaitPanel::changeEvent(QEvent *event)
{
    if(event->type() == QEvent::LanguageChange)
    {
        retranslateUi();
    }

    QWidget::changeEvent(event);
}

void WaitPanel::retranslateUi()
{
   m_promptLabel->setText(POWERLINE_GET_TEXT(L_MAP_WAITSCANNING,"Scanning ,please wait..."));
}

QString WaitPanel::translateText(int idx, const QString &defaultText)
{
    QString text;
    emit translateText(idx,&text);
    return text.isEmpty() ? defaultText : text;
}

void WaitPanel::start()
{
    if(!m_indicator->isAnimated())
    {
        m_indicator->startAnimation();
    }
}

void WaitPanel::stop()
{
    if(m_indicator->isAnimated())
    {
        m_indicator->stopAnimation();
    }
}
