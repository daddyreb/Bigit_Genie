#include "QGenieChooseLocationWidget.h"
#include "QGenieDialogContainerFrame.h"

//#include "plugin_ui_def.h"
#include "Genieplugin_Internet.h"

#include <assert.h>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpacerItem>


QGenieChooseLocationWidget::QGenieChooseLocationWidget(QWidget *parent)
    :QGenieDialogInnerWidget(parent)
{
    this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    locations_list  = new QComboBox(this);

    prompt_label = new QLabel(this);
    ok_btn	 = new QPushButton(this);
    cancel_btn	 = new QPushButton(this);

    prompt_label->setWordWrap(true);

    //SET_STYLE_PROPERTY(FLEX_BUTTON,ok_btn);
    //SET_STYLE_PROPERTY(FLEX_BUTTON,cancel_btn);

    QHBoxLayout	*hor_layout=new QHBoxLayout();
    QSpacerItem *sp_item=new QSpacerItem(0,0,QSizePolicy::Expanding);
    hor_layout->addItem(sp_item);
    hor_layout->addWidget(ok_btn);
    hor_layout->addWidget(cancel_btn);

    QVBoxLayout *ver_layout=new QVBoxLayout(this);
    ver_layout->addWidget(prompt_label);
    ver_layout->addWidget(locations_list);
    ver_layout->addLayout(hor_layout);

    setSizeIncrement(0,0);

    retranslateUi();

    connect(ok_btn,SIGNAL(clicked(bool)),this,SLOT(ok_btn_clicked(bool)));
    connect(cancel_btn,SIGNAL(clicked(bool)),this,SLOT(cancel_btn_clicked(bool)));

}

QGenieChooseLocationWidget::~QGenieChooseLocationWidget()
{
}

void QGenieChooseLocationWidget::retranslateUi()
{

    prompt_label->setText(GeniePlugin_Internet::translateText(MACX_L_INTERNET_CLW_PROMPT));
    ok_btn->setText(GeniePlugin_Internet::translateText(L_INTERNET_OK_BTN_TEXT));
    cancel_btn->setText(GeniePlugin_Internet::translateText(L_INTERNET_PCW_CANCEL_BTN_TEXT));
    this->setWindowTitle(GeniePlugin_Internet::translateText(MACX_L_INTERNET_CLW_TITLE));

}

void QGenieChooseLocationWidget::changeEvent(QEvent *event)
{
    if(event->type()==QEvent::LanguageChange)
        retranslateUi();
    QGenieDialogInnerWidget::changeEvent(event);
}

void QGenieChooseLocationWidget::resizeEvent(QResizeEvent *event)
{
    setFixedSize(event->size());
}

void QGenieChooseLocationWidget::ok_btn_clicked(bool checked)
{
    if ( locations_list->currentIndex() != -1 )
    {
        accept();
    }
    else
    {
        QGenieDialogContainerFrame::messageBox(this,get_dlg_title(),
                                               GeniePlugin_Internet::translateText(MACX_L_INTERNET_CLW_CBLIST_NOT_SELECTED),false);
    }
}

void QGenieChooseLocationWidget::cancel_btn_clicked(bool checked)
{
    reject();
}

QString QGenieChooseLocationWidget::get_dlg_title()
{
    return GeniePlugin_Internet::translateText(MACX_L_INTERNET_CLW_TITLE);
}

void QGenieChooseLocationWidget::init_location_list(const QStringList &sets)
{
   assert ( sets.size() % 2 == 0 );
   locations_list->clear();

   for (int k = 0 ; k < sets.size()-1 ; k +=2)
   {
        locations_list->addItem(sets[k],sets[k+1]);
   }

   if( locations_list->count() != 0 )
   {
        locations_list->setCurrentIndex(0);
   }
}

QString QGenieChooseLocationWidget::get_location()
{
    int curidx = locations_list->currentIndex();
    QString result;

    if( curidx != -1 )
    {
        result = locations_list->itemData(curidx,Qt::UserRole).toString();
    }

    return result;
}
