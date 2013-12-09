#include "QGenieAvailableNetworkList.h"
#include <QHBoxLayout>
#include <QListWidgetItem>

QGenieAvailableNetworkList::QGenieAvailableNetworkList(QWidget *parent) :
    QFrame(parent)
{
    network_list  = new QGenieListWidget(this);
//    network_list->setRootIsDecorated(false);
    network_list->setAlternatingRowColors(true);
//    network_list->setHeaderHidden(true);
    network_list->setObjectName(QString("WlanVisibleNetworkList"));
    //network_list->setFocusPolicy(Qt::NoFocus);
    network_list->setStyleSheet("QListView::item:selected{background-color:rgb(49,106,197)}");

    cur_selected_item  = 0;

    QHBoxLayout     *hor_layout = new QHBoxLayout(this);
    hor_layout->setMargin(0);
    hor_layout->addWidget(network_list);

    connect(network_list,SIGNAL(itemSelectionChanged()),this,SLOT(on_itemSelectionChanged()));
    connect(network_list,SIGNAL(itemSelectionChanged()),this,SIGNAL(itemSelectionChanged()));
    connect(network_list,SIGNAL(itemDoubleClicked(QListWidgetItem*)),this,SIGNAL(itemDoubleClicked (QListWidgetItem *)));
}

void QGenieAvailableNetworkList::clear()
{
    cur_selected_item = 0;
    int itemCount = network_list->count();
    QListWidgetItem *twi = 0;
    QWidget *itemWidget = 0;
    for(int i = 0 ; i < itemCount ; ++i){
        twi = network_list->item(i);
        itemWidget = network_list->itemWidget(twi);
        network_list->removeItemWidget(twi);
        delete itemWidget;
    }
    network_list->clear();
}

void QGenieAvailableNetworkList::add_networkitem(const QString &ssid,const QString &bss_type,
                                                 const QString &auth,const QString &encry,bool sec_enabled,
                                                 int connectstate,int signal_strength,int channel,
                                                 bool hasprofile,const QString &strprofile)
{
    QGenieAvailableNetworkItem *ani = new QGenieAvailableNetworkItem(network_list);
    ani->set_ssid(ssid);
    ani->set_bss_type(bss_type);
    ani->set_auth_algorithm(auth);
    ani->set_encry_algorithm(encry);
    ani->set_sec_enabled(sec_enabled);
    ani->set_curconnected(connectstate);
    ani->set_signal_strength(signal_strength);
    ani->set_channel(channel);
    ani->set_hasprofile(hasprofile);
    ani->set_profile(strprofile);

    QListWidgetItem *twi = new QListWidgetItem(0);
    twi->setSizeHint(QSize(580,59));
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

QGenieAvailableNetworkItem *QGenieAvailableNetworkList::get_item_byssid(const QString &ssid)
{
    QListWidgetItem *item = 0;
    QGenieAvailableNetworkItem *networkitem = 0;

    for(int i=0;i<network_list->count();++i)
    {
        item = network_list->item(i);
        if(item)
        {
            networkitem = qobject_cast<QGenieAvailableNetworkItem*>( network_list->itemWidget(item));
            if(networkitem && (!networkitem->get_ssid().compare(ssid,Qt::CaseSensitive)))
            {
                return networkitem;
            }
        }
    }

    return 0;

}
