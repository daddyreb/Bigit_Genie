#ifndef QGENIESTATICINTERFACE_H
#define QGENIESTATICINTERFACE_H

#include <QWidget>
#include "Singleton.h"

namespace Ui {
class QGenieStaticInterface;
}

class QGenieStaticInterface : public QWidget,public Singleton<QGenieStaticInterface>
{
    Q_OBJECT

public:
    explicit QGenieStaticInterface(QWidget *parent = 0);
    ~QGenieStaticInterface();
    void updateUi();
protected:
    void updateRouteTypeData();
private slots:
    void on_comboBox_currentIndexChanged(int index);



    void on_sendclick_clicked();

    void on_sendinstdata_clicked();

    void on_sendrouterdata_clicked();

    void on_sendsearch_clicked();

    void on_pagebut_clicked();

    void on_getinfo_clicked();

    void on_getrouteinfo_clicked();

private:
    Ui::QGenieStaticInterface *ui;
    friend class QGenieDataContainner;
};

#endif // QGENIESTATICINTERFACE_H
