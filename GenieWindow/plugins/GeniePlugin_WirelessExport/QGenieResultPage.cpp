#include "QGenieResultPage.h"
#include "GeniePlugin_WSetupPlugin.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSpacerItem>
#include <QPixmap>

QGenieResultPage::QGenieResultPage(QWidget *parent) :
        QFrame(parent)
{
    m_bsuccess =false;

    result_text_lb       = new QLabel(this);
    result_image_lb      = new QLabel(this);
    end_btn              = new QPushButton(this);

    result_image_lb->setPixmap(QPixmap(":/image/Ok.png"));

    QLabel *space_button = new QLabel(this);
    space_button->setObjectName(QString("SpaceButton"));
    QHBoxLayout     *btn_layout  = new QHBoxLayout();
    btn_layout->setSpacing(20);
    btn_layout->addStretch(1);
    btn_layout->addWidget(end_btn);
    btn_layout->addWidget(space_button);

    QHBoxLayout     *result_image_layout = new QHBoxLayout();
    result_image_layout->addStretch(1);
    result_image_layout->addWidget(result_image_lb);
    result_image_layout->addStretch(1);

    QHBoxLayout     *result_text_layout = new QHBoxLayout();
    result_text_layout->addStretch(1);
    result_text_layout->addWidget(result_text_lb);
    result_text_layout->addStretch(1);

    QSpacerItem       *ver_sp = new QSpacerItem(0,40,QSizePolicy::Minimum,QSizePolicy::Fixed);
    QSpacerItem       *ver_sp_down = new QSpacerItem(0,60,QSizePolicy::Minimum,QSizePolicy::Fixed);
    QVBoxLayout      *page_layout = new QVBoxLayout(this);
    page_layout->addStretch(1);
    page_layout->addLayout(result_image_layout);
    page_layout->addItem(ver_sp);
    //     page_layout->addWidget(result_text_lb);
    page_layout->addLayout(result_text_layout);
    page_layout->addItem(ver_sp_down);
    page_layout->addLayout(btn_layout);

    connect(end_btn,SIGNAL(clicked()),this,SIGNAL(resultpage_end_btn_clicked()));
    retranslateUi();
}

void QGenieResultPage::init(bool bsuccess)
{
    if(bsuccess)
    {
        result_image_lb->setPixmap(QPixmap(":/image/Ok.png"));
        //statistics
        GeniePlugin_WSetupPlugin::fun_statistics("R4");
        //end
    }
    else
    {
        result_image_lb->setPixmap(QPixmap(":/image/Failed.png"));
    }

    m_bsuccess = bsuccess;
    retranslateUi();
}

void QGenieResultPage::changeEvent(QEvent *event)
{
    if(event->type()==event->LanguageChange)
    {
        retranslateUi();
    }
    QFrame::changeEvent(event);
}

void QGenieResultPage::retranslateUi()
{
    if(m_bsuccess)
    {
        result_text_lb->setText(/*tr("Export wireless configureture to flash disk ok.")*/
                GeniePlugin_WSetupPlugin::translateText(L_WIRELESSEXPORT_RP_EXPORT_WLAN_OK));
    }
    else
    {
        result_text_lb->setText(/*tr("Export wireless configureture to flash disk failed.")*/
                GeniePlugin_WSetupPlugin::translateText(L_WIRELESSEXPORT_RP_EXPORT_WLAN_FAILED));
    }
    end_btn->setText(/*tr("Finish")*/
            GeniePlugin_WSetupPlugin::translateText(L_WIRELESSEXPORT_RP_FINISH_BTN_TEXT));
}
