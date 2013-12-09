#include "QGenieDialogInnerWidget.h"

QGenieDialogInnerWidget::QGenieDialogInnerWidget(QWidget *parent) :
    QWidget(parent),m_container_dlg(0)
{

}

void  QGenieDialogInnerWidget::set_container_dlg(QDialog *dlg)
{
    if(0==dlg)
        return;
    m_container_dlg = dlg;
}

void  QGenieDialogInnerWidget::accept()
{
    if(m_container_dlg)
        m_container_dlg->accept();
}

void  QGenieDialogInnerWidget::reject()
{
    if(m_container_dlg)
        m_container_dlg->reject();
}
