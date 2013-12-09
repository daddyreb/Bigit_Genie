#include "QMyWidget.h"
#include "ui_QMyWidget.h"
#include "plugin_ui_def/plugin_ui_def.h"

QMyWidget::QMyWidget(QWidget *parent) :
    QGenieBaseWidget(parent),
    ui(new Ui::QMyWidget)
{
    ui->setupUi(this);
    SET_STYLE_PROPERTY(FLEX_BUTTON,ui->testbutton);
}

QMyWidget::~QMyWidget()
{
    delete ui;
}

void QMyWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    if(e->type() ==QEvent::LanguageChange)
    {
        ui->label->setText(getI18nLan(1901));

        QString strOk=getI18nLan(1902);
        QString style=QString(BBT_MODE_CLICKHERE).arg(strOk);
        setBigButtonStyle(style);
    }
   
}
