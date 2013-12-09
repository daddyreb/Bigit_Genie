#ifndef TESTTOPOLOGYFORM_H
#define TESTTOPOLOGYFORM_H

#include <QWidget>
class SVT_Core;
#include "topology.h"

class SVTController;

namespace Ui {
    class TestTopologyForm;
}

class TestTopologyForm : public QWidget
{
    Q_OBJECT

public:
    explicit TestTopologyForm(QWidget *parent = 0);
    ~TestTopologyForm();
//    SVT_Core *mSVTCore;
    SVT_ControlProxy *mController;
    void subscribeCurpage();
private:
    void initHeaders();
    Ui::TestTopologyForm *ui;
//    int mCurrentPage;
//    int mCurrentIface;
    int mAllCount;
    QMap<int,int> mIfaceMap;
protected slots:
    void slot_DataReturned(SVT_NotifyType);
    void slot_IfaceReturned(SVT_NotifyType);
    void slot_NewDeviceAdded(SVT_NotifyType);
    void slot_CountChanged(int c);
    void slot_GatewayChanged(QString gateway);
    void slot_NetgearInfoReturned(const QMap<int,QString>& );
private slots:
    void on_checkBox_fupdate_clicked(bool checked);
    void on_but_last_clicked();
    void on_but_next_clicked();
    void on_comboBox_currentIndexChanged(int index);
    void on_pushButton_clicked();
    void on_checkBox_upnp_clicked(bool checked);
    void on_checkBox_currentsetting_clicked(bool checked);
    void on_checkBox_attachdevice_clicked(bool checked);
    void on_checkBox_netbios_clicked(bool checked);
    void on_checkBox_flags_clicked(bool checked);
    void on_checkBox_all_clicked(bool checked);
    void on_checkBox_local_clicked(bool checked);
    void on_pushButton_refresh_clicked();
    void on_but_netgearinfo_clicked();
};

#endif // TESTTOPOLOGYFORM_H
