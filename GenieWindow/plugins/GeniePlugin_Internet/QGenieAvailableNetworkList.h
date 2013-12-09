#ifndef QGENIEAVAILABLENETWORKLIST_H
#define QGENIEAVAILABLENETWORKLIST_H
#include "QGenieAvailableNetworkItem.h"
#include <QFrame>
#include <QListWidget>

class QGenieAvailableNetworkList : public QFrame
{
Q_OBJECT
public:
    explicit QGenieAvailableNetworkList(QWidget *parent = 0);
protected:
    QListWidget                 *network_list;
    QGenieAvailableNetworkItem    *cur_selected_item;
public:
    void clear();
    void add_networkitem(const QString &ssid,const QString &bss_type,
                         const QString &auth,const QString &encry,bool sec_enabled,int signal_strength,int channel);
    QGenieAvailableNetworkItem *get_selected_item();
signals:
   void  itemSelectionChanged();
   void  itemDoubleClicked( QListWidgetItem *item);
public slots:
   void on_itemSelectionChanged();

};

#endif // QGENIEAVAILABLENETWORKLIST_H
