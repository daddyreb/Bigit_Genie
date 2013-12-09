#include "QGenieProfileListItemWidget.h"
#include "ui_QGenieProfileListItemWidget.h"

#include <QtGlobal>

QGenieProfileListItemWidget::QGenieProfileListItemWidget(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::QGenieProfileListItemWidget)
{
    ui->setupUi(this);
}

QGenieProfileListItemWidget::~QGenieProfileListItemWidget()
{
    delete ui;
}

void QGenieProfileListItemWidget::setProfileConnType(bool binfrastructure)
{
    m_connType = binfrastructure;
    ui->m_profileImageLB->setPixmap(binfrastructure?QPixmap(":/image/I_WNetwork.png"):QPixmap(":/image/A_WNetwork.png"));
}

bool QGenieProfileListItemWidget::profileConnType()
{
    return m_connType;
}

void QGenieProfileListItemWidget::setProfileName(const QString &name)
{
    ui->m_profileNameLB->setText(name);
}

void QGenieProfileListItemWidget::setProfileSecurity(const QString &security)
{
    ui->m_profileSecurityLB->setText(security);
}

void QGenieProfileListItemWidget::exchangeUi(QGenieProfileListItemWidget *itemWidget)
{
    Q_ASSERT(itemWidget);

    QPixmap pixmap;
    if(ui->m_profileImageLB->pixmap())
    {
        pixmap = *ui->m_profileImageLB->pixmap();
    }

    ui->m_profileImageLB->setPixmap(itemWidget->ui->m_profileImageLB->pixmap()?
                                    *itemWidget->ui->m_profileImageLB->pixmap():QPixmap());
    itemWidget->ui->m_profileImageLB->setPixmap(pixmap);

    QString profileName = ui->m_profileNameLB->text();
    ui->m_profileNameLB->setText(itemWidget->ui->m_profileNameLB->text());
    itemWidget->ui->m_profileNameLB->setText(profileName);

    QString profileSecurity = ui->m_profileSecurityLB->text();
    ui->m_profileSecurityLB->setText(itemWidget->ui->m_profileSecurityLB->text());
    itemWidget->ui->m_profileSecurityLB->setText(profileSecurity);
}
