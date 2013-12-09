#ifndef AIRPRINTERWIDGET_H
#define AIRPRINTERWIDGET_H

#include <QWidget>
#include <QMap>
#include <QTableWidgetItem>
#include <QMovie>


#include "airprintbackgroud.h"
#include "QProgressIndicator.h"

namespace Ui {
    class AirprintWidget;
}

class AirprintWidget : public QWidget {
    Q_OBJECT

public:
    AirprintWidget(QWidget *parent = 0);
    ~AirprintWidget();

protected:
    void changeEvent(QEvent *e);
    void retranslateUi();
    void timerEvent(QTimerEvent *);

private:
    Ui::AirprintWidget *ui;
//    QFont font;
    QPixmap *uninstallationpixmap;
    AirprintBackgroud *airprintbackgroud;
    QMap<QString , QTableWidgetItem *> printernamecheckboxitemmap;
    QMovie *movie;
    int timerId;
    QProgressIndicator * testpagestatus;
    QProgressIndicator * printerlistpagestatus;

    void readConfig(QMap<QString,bool> &);
    void writeConfig(QMap<QString,bool> );

    void setMessage(QString &msg);

signals:
    void sendLockMainWindow();
    void sendUnlockMainWindow();

public slots:
    void recevieStatus(short status);
    void receviePrinterNameList(QMap<QString,bool>);
    void recevieSaveConfig(QMap<QString,bool>);
    void recevicePrinterStatus(QString,Qt::CheckState);
    void receviceNeedWriteConfig();
    void refeshBtClicked();

    void receviceLockMainWindow();
    void receviceUnlockMainwindow();

    void recevieMessage(QString , bool);

private slots:
    void itemClicked(QTableWidgetItem * item);
};
#endif // AIRPRINTERWIDGET_H
