#ifndef TOOLS_H
#define TOOLS_H

#include <QtGui>
#include "ping.h"
#include "traceroute.h"
#include "dnslookup.h"
#include "profile.h"
#include "QProgressIndicator.h"
#include "pushbutton.h"

class Tools : public QWidget
{
    Q_OBJECT
public:
    Tools(QWidget* parent);
    ~Tools();

private:
    void retranslateUi();

private:
    QGridLayout *gridLayout_4;
    QFrame *frame;
    QGridLayout *gridLayout;
    QLabel *label;
    QSpacerItem *verticalSpacer_3;
//    QFrame *line;
    QFrame *frame_2;
    QGridLayout *gridLayout_2;
    QSpacerItem *verticalSpacer;
    PushButton *pushButton;
    PushButton *pushButton_2;
    PushButton *pushButton_3;
    PushButton *pushButton_4;
    QSpacerItem *verticalSpacer_2;
    QFrame *frame_3;
    QStackedLayout* stackedLayout;
    Ping* ping;
    TraceRoute* route;
    DNSLookup* dnslookup;
    Profile* pro;

    QProgressIndicator* pi;
    QUuid uuid;

private slots:
    void pingchange();
    void routechange();
    void dnschange();
    void profilechange();
    void getinfoprofile();
//    void saveping();
//    void savepinged();

signals:
    void getinfo();

protected:
    void changeEvent(QEvent* event);
    friend class MainWindow;
};

#endif // TOOLS_H
