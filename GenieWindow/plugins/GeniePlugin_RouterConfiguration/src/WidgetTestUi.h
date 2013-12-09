#ifndef WIDGETTESTUI_H
#define WIDGETTESTUI_H

#include <QWidget>
#include "QGenieRCPageWireless.h"
//#include "wirelessinfo.h"
//#include "wirelesssettings.h"

namespace Ui {
    class WidgetTestUi;
}
class QGenieRCPageWireless;
class WirelessInfo;
class WirelessSettings;
class WidgetTestUi : public QWidget {
    Q_OBJECT
public:
    WidgetTestUi(QWidget *parent = 0);
    ~WidgetTestUi();
//
//    QGenieRCPageWireless *mWireless;
//    WirelessInfo *mWirelessInfo;
//    WirelessSettings *mWirelessSetting;

protected:
    void changeEvent(QEvent *e);
protected slots:
    void on_but1_clicked();
    void on_but2_clicked();
    void on_but3_clicked();
    void on_but4_clicked();
	void on_but5_clicked();
private slots:
        void on_but6_clicked();

        void on_but7_clicked();

private:
    Ui::WidgetTestUi *ui;
};

#endif // WIDGETTESTUI_H
