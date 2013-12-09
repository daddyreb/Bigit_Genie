#include "QGenieWiFiProfileWidget.h"
#include "ui_QGenieWiFiProfileWidget.h"
#include "GeniePlugin_Wireless.h"
#include "QGenieWlanKeyPersistent.h"
#include "genie2_interface.h"

#ifdef Q_OS_WIN
#include "QGenieWiFiProfileManagerController.h"
#elif defined Q_OS_MACX
#include "QGenieWiFiUtilityControllerForMacx.h"
#endif
#include "QGenieConfirmMessageBox.h"
#include "QGenieProfileListItemWidget.h"

#include <QEvent>
#include <QList>
#include <QPixmap>
#include <QIcon>
#include <QVariant>
#include <QtDebug>

QGenieWiFiProfileWidget::QGenieWiFiProfileWidget(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::QGenieWiFiProfileWidget),
        m_selectedRow(-1)
{
    ui->setupUi(this);

    ui->refreshBtn->setVisible(false);
    ui->profileList->setAlternatingRowColors(true);
    ui->profileList->setObjectName(QString("WlanVisibleNetworkList"));
    ui->profileList->setStyleSheet("QListView::item:selected{background-color:rgb(49,106,197)}");

    connect(ui->moveDownBtn,SIGNAL(clicked()),this,SLOT(btnClicked()));
    connect(ui->moveUpBtn,SIGNAL(clicked()),this,SLOT(btnClicked()));
    connect(ui->delProfileBtn,SIGNAL(clicked()),this,SLOT(btnClicked()));
    connect(ui->refreshBtn,SIGNAL(clicked()),this,SLOT(btnClicked()));
    connect(ui->profileList,SIGNAL(itemSelectionChanged()),
            this,SLOT(itemSelectionChanged()));

    connect(ui->addProfileBtn,SIGNAL(clicked()),this,SLOT(btnClicked()));
    connect(ui->backBtn,SIGNAL(clicked()),this,SLOT(btnClicked()));

    retranslateUi();
}

QGenieWiFiProfileWidget::~QGenieWiFiProfileWidget()
{
    delete ui;
}

void QGenieWiFiProfileWidget::btnClicked()
{
    QPushButton *sender = qobject_cast<QPushButton *>(this->sender());

    if(sender == ui->moveUpBtn)
    {
        moveUpProfile();
    }
    else if(sender == ui->moveDownBtn)
    {
        moveDownProfile();
    }
    else if(sender == ui->delProfileBtn)
    {
        deleteProfile();
    }
    else if(sender == ui->refreshBtn)
    {
        reloadData();
    }
    else if(sender == ui->addProfileBtn)
    {
        emit requestAddProfile();
    }
    else if(sender == ui->backBtn)
    {
        emit networkList();
    }

    updateUiElements();
}


void QGenieWiFiProfileWidget::itemSelectionChanged()
{
    QGenieProfileListItemWidget *itemWidget = NULL;

    QList<QListWidgetItem *> items = ui->profileList->selectedItems();

    int newRow = ((items.size() > 0)?ui->profileList->count():-1);

    if((m_selectedRow >= 0) && (m_selectedRow < ui->profileList->count()))
    {
        itemWidget = qobject_cast<QGenieProfileListItemWidget*>(ui->profileList->itemWidget(ui->profileList->item(m_selectedRow)));
        itemWidget->setStyleSheet(QString::fromUtf8("color:rgb(0,0,0)"));
    }

    if((newRow >= 0) && (newRow < ui->profileList->count()))
    {

        itemWidget = qobject_cast<QGenieProfileListItemWidget*>(ui->profileList->itemWidget(ui->profileList->item(m_selectedRow)));
        itemWidget->setStyleSheet(QString::fromUtf8("color:rgb(255,255,255)"));
    }

    m_selectedRow = newRow;

    updateUiElements();
}

void QGenieWiFiProfileWidget::deleteProfile()
{
    QStringList dialog_lans;
    dialog_lans<<GENIE2_GET_TEXT(L_WIRELESS_CMB_TITLE)<<GENIE2_GET_TEXT(L_WIRELESS_CMB_CONTENT);

    bool btemp=GENIE2_UI_COMMAND(UICMD_DIALOG_MODAL,dialog_lans).toBool();
    if(!btemp)
    {
        return;
    }


    QList<QListWidgetItem *> selectedItems = (ui->profileList->selectedItems());
    if(selectedItems.size() <= 0)
    {
        return;
    }

    int row     = ui->profileList->indexFromItem(selectedItems[0]).row();
    int count   = ui->profileList->count();

    if(row != -1)
    {
        if(g_WifiProfileManager.deleteProfile(row))
        {
            if(m_selectedRow == row)
            {
                m_selectedRow = -1;
            }

            delete ui->profileList->itemWidget(selectedItems[0]);

            QListWidgetItem *item2del = ui->profileList->item(row);
            //remove saved wireless password
            QGenieWlanKeyPersistent::save_key(item2del->data(Qt::UserRole).toString(),QString());

            delete item2del;

            if(row < (count -1))
            {
                m_selectedRow = row;
            }
            else if(row > 0)
            {
                m_selectedRow = row -1;
            }

            if((m_selectedRow >= 0) && (m_selectedRow < (count - 1)))
            {
                ui->profileList->setCurrentItem(ui->profileList->item(m_selectedRow));
            }
        }
        else
        {
            init();//re-initilize profile list
        }
    }
}

void QGenieWiFiProfileWidget::exchangeProfileListItem(int row1,int row2)
{
    QListWidgetItem *item1 = ui->profileList->item(row1);
    QListWidgetItem *item2 = ui->profileList->item(row2);

    QGenieProfileListItemWidget *itemWidget1 = qobject_cast<QGenieProfileListItemWidget *>(ui->profileList->itemWidget(item1));
    QGenieProfileListItemWidget *itemWidget2 = qobject_cast<QGenieProfileListItemWidget *>(ui->profileList->itemWidget(item2));
    itemWidget1->exchangeUi(itemWidget2);

    QVariant tmpData = item1->data(Qt::UserRole);
    item1->setData(Qt::UserRole,item2->data(Qt::UserRole));
    item2->setData(Qt::UserRole,tmpData);
}

void QGenieWiFiProfileWidget::moveUpProfile()
{
    QList<QListWidgetItem *> selectedItems = (ui->profileList->selectedItems());
    if(selectedItems.size() <= 0)
    {
        return;
    }

    int row = ui->profileList->indexFromItem(selectedItems[0]).row();

    if(row > 0)
    {
        if(g_WifiProfileManager.moveUpProfile(row))
        {
            exchangeProfileListItem(row,row - 1);
            ui->profileList->setCurrentItem(ui->profileList->item(row - 1));

        }
        else
        {
            init();//re-initilize profile list
        }
    }
}

void QGenieWiFiProfileWidget::moveDownProfile()
{
    QList<QListWidgetItem *> selectedItems = (ui->profileList->selectedItems());
    if(selectedItems.size() <= 0)
    {
        return;
    }

    int row = ui->profileList->indexFromItem(selectedItems[0]).row();

    if((row != -1) && (row < (ui->profileList->count() - 1)))
    {
        if(g_WifiProfileManager.moveDownProfile(row))
        {
            exchangeProfileListItem(row,row + 1);
            ui->profileList->setCurrentItem(ui->profileList->item(row + 1));
        }
        else
        {
            init();//re-initilize profile list
        }
    }
}


void QGenieWiFiProfileWidget::updateUiElements()
{
    QList<QListWidgetItem *> selectedItems = (ui->profileList->selectedItems());

    int row     = ((selectedItems.size() <= 0)?-1:ui->profileList->indexFromItem(selectedItems[0]).row());
    int count   = ui->profileList->count();

    ui->delProfileBtn->setEnabled(row != -1);

    bool canMove = false;

    if(selectedItems.size() > 0)
    {
        QGenieProfileListItemWidget *itemWidget = qobject_cast<QGenieProfileListItemWidget*>(ui->profileList->itemWidget(selectedItems[0]));
        if(itemWidget)
        {
            canMove = itemWidget->profileConnType();
        }
    }

    ui->moveDownBtn->setEnabled((row != -1) && (row < (count - 1)) && canMove);
    ui->moveUpBtn->setEnabled((row > 0) && canMove);
}

void QGenieWiFiProfileWidget::addProfileCompleted(bool added)
{
    init();
}

bool QGenieWiFiProfileWidget::reloadData()
{
    bool bok = false;

    ui->profileList->clear();
    m_selectedRow = -1;

    QListWidgetItem *listItem = NULL;

    QStringList profileList;
    bok = g_WifiProfileManager.updateProfileList(profileList);

    QString auth;
    bool    binfrastructure;

    foreach(QString profile, profileList)
    {
        g_WifiProfileManager.getProfileAuthAndConnType(profile,auth,binfrastructure);

        listItem = new QListWidgetItem(ui->profileList);
        listItem->setData(Qt::UserRole,profile);
        ui->profileList->addItem(listItem);
        listItem->setSizeHint(QSize(520,59));

        QGenieProfileListItemWidget *itemWidget = new QGenieProfileListItemWidget(ui->profileList);
        itemWidget->setProfileConnType(binfrastructure);
        itemWidget->setProfileName(profile);
        itemWidget->setProfileSecurity(auth);

        ui->profileList->setItemWidget(listItem,itemWidget);
    }

    if(!bok)
    {
        emit error();
    }

    return bok;
}

void QGenieWiFiProfileWidget::init()
{
    reloadData();
    updateUiElements();
}

void QGenieWiFiProfileWidget::retranslateUi()
{
    ui->wifiProfileLabel->setText(GeniePlugin_Wireless::translateText(L_WIRELESS_WPW_PROFILELIST_TITLE));
    ui->wifiProfileLabel2->setText(GeniePlugin_Wireless::translateText(L_WIRELESS_WPW_PROFILELIST_TITLE2));
    ui->refreshBtn->setText(GeniePlugin_Wireless::translateText(L_WIRELESS_VNW_REFRESH_BTN_TEXT));
    ui->moveDownBtn->setText(GeniePlugin_Wireless::translateText(L_WIRELESS_WPW_MOVEDOWN)/*"MoveDown"*/);
    ui->moveUpBtn->setText(GeniePlugin_Wireless::translateText(L_WIRELESS_WPW_MOVEUP)/*"MoveUp"*/);
    ui->delProfileBtn->setText(GeniePlugin_Wireless::translateText(L_WIRELESS_WPW_DELETE)/*"Delete"*/);
    ui->addProfileBtn->setText(GeniePlugin_Wireless::translateText(L_WIRELESS_WPW_ADD)/*"Add"*/);
    ui->backBtn->setText(GeniePlugin_Wireless::translateText(L_WIRELESS_WPW_BACK)/*"Back"*/);
}

void QGenieWiFiProfileWidget::changeEvent(QEvent *event)
{
    if(event->type() == QEvent::LanguageChange)
        retranslateUi();

    QWidget::changeEvent(event);
}
