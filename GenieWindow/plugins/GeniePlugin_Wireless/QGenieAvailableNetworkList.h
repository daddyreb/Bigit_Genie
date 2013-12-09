#ifndef QGENIEAVAILABLENETWORKLIST_H
#define QGENIEAVAILABLENETWORKLIST_H
#include "QGenieAvailableNetworkItem.h"
#include <QFrame>
//#include <QTreeWidget>
#include "QGenieListWidget.h"

class QGenieAvailableNetworkList : public QFrame
{
Q_OBJECT
public:
    explicit QGenieAvailableNetworkList(QWidget *parent = 0);
protected:
    QGenieListWidget                   *network_list;
    QGenieAvailableNetworkItem      *cur_selected_item;
public:
    void clear();
    void add_networkitem(const QString &ssid,const QString &bss_type,
     const QString &auth,const QString &encry,bool sec_enabled,int connectstate,
     int signal_strength,int channel,bool hasprofile=false,
     const QString &strprofile =QString(""));

    QGenieAvailableNetworkItem *get_selected_item();
    QGenieAvailableNetworkItem *get_item_byssid(const QString &ssid);
signals:
   void  itemSelectionChanged();
   void  itemDoubleClicked ( QListWidgetItem *item);
public slots:
   void on_itemSelectionChanged();

};

#endif // QGENIEAVAILABLENETWORKLIST_H
