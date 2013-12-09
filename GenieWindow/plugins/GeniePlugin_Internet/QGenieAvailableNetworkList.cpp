#include "QGenieAvailableNetworkList.h"
#include <QHBoxLayout>
#include <QTreeWidgetItem>

QGenieAvailableNetworkList::QGenieAvailableNetworkList(QWidget *parent) :
        QFrame(parent)
{
    network_list  = new QListWidget(this);
//    network_list->setRootIsDecorated(false);
    network_list->setAlternatingRowColors(true);
//    network_list->setHeaderHidden(true);
    network_list->setObjectName(QString("WlanVisibleNetworkList"));
    network_list->setStyleSheet( /*"QTreeView::item:hover{background-color:rgb(0,255,0,50)}"*/
                                                 "QListView::item:selected{background-color:rgb(49,106,197)}");
    //network_list->setFocusPolicy(Qt::NoFocus);

    cur_selected_item  = 0;

    QHBoxLayout     *hor_layout = new QHBoxLayout(this);
    hor_layout->setMargin(0);
    hor_layout->addWidget(network_list);

    connect(network_list,SIGNAL(itemSelectionChanged()),this,SLOT(on_itemSelectionChanged()));
    connect(network_list,SIGNAL(itemSelectionChanged()),this,SIGNAL(itemSelectionChanged()));
    connect(network_list,SIGNAL(itemDoubleClicked(QListWidgetItem*)),this,SIGNAL(itemDoubleClicked(QListWidgetItem *)));
}

void QGenieAvailableNetworkList::clear()
{
    cur_selected_item = 0;
    network_list->clear();
}

void QGenieAvailableNetworkList::add_networkitem(const QString &ssid,const QString &bss_type,
                                                 const QString &auth,const QString &encry,bool sec_enabled,int signal_strength,int channel)
{
    QGenieAvailableNetworkItem *ani = new QGenieAvailableNetworkItem(network_list);
    ani->set_ssid(ssid);
    ani->set_bss_type(bss_type);
    ani->set_auth_algorithm(auth);
    ani->set_encry_algorithm(encry);
    ani->set_sec_enabled(sec_enabled);
    ani->set_signal_strength(signal_strength);
    ani->set_channel(channel);

    QListWidgetItem *twi = new QListWidgetItem(0);
    twi->setSizeHint(QSize(400,59));
    network_list->addItem(twi);
    network_list->setItemWidget(twi,ani);
}

QGenieAvailableNetworkItem *QGenieAvailableNetworkList::get_selected_item()
{
    QList<QListWidgetItem*> items = network_list->selectedItems();
    if(items.size()>0)
    {
        return qobject_cast<QGenieAvailableNetworkItem*>( network_list->itemWidget(items[0]));
    }
    else
    {
        return 0;
    }
}

void QGenieAvailableNetworkList::on_itemSelectionChanged()
{
    QGenieAvailableNetworkItem *item = get_selected_item();
    if(item)
    {
        if(cur_selected_item)
        {
            cur_selected_item->setStyleSheet(QString::fromUtf8("corlor:rgb(0,0,0)"));
        }
        item->setStyleSheet(QString::fromUtf8("color:rgb(255,255,255)"));
        cur_selected_item = item;
    }
}
