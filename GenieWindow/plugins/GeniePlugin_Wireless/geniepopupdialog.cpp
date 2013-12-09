#include "geniepopupdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <assert.h>


GeniePopupDialog::GeniePopupDialog(QWidget *parent) :
        ScalableDialogContentWidget(parent)
{
    connect(m_container_dlg,SIGNAL(accepted()),this,SIGNAL(accepted()));
    connect(m_container_dlg,SIGNAL(rejected()),this,SIGNAL(rejected()));
    connect(m_container_dlg,SIGNAL(finished(int)),this,SIGNAL(finished(int)));
}

void GeniePopupDialog::show()
{
    m_container_dlg->show();
}

void GeniePopupDialog::setWindowTitle(const QString &title)
{
    m_container_dlg->setFrameTitle(title);
}

bool GeniePopupDialog::isSizeGripEnabled() const
{
    return m_container_dlg->isSizeGripEnabled();
}

int GeniePopupDialog::result() const
{
    return m_container_dlg->result();
}

void GeniePopupDialog::setModal(bool modal)
{
    m_container_dlg->setModal(modal);
}

void GeniePopupDialog::setResult (int i)
{
    m_container_dlg->setResult(i);
}

void GeniePopupDialog::setSizeGripEnabled (bool enable)
{
    m_container_dlg->setSizeGripEnabled(enable);
}


void  GeniePopupDialog::accept()
{
    if(m_container_dlg)
        m_container_dlg->accept();
}

void  GeniePopupDialog::reject()
{
    if(m_container_dlg)
        m_container_dlg->reject();
}


void GeniePopupDialog::done(int r)
{
    m_container_dlg->done(r);
}

int GeniePopupDialog::exec()
{
    return m_container_dlg->exec();
}

void GeniePopupDialog::open()
{
    m_container_dlg->open();
}
