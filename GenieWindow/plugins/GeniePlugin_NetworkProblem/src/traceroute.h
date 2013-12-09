#ifndef TRACEROUTE_H
#define TRACEROUTE_H

#include <QtGui>
#include <QtWidgets>
#include "../../DragonNetTool/interface/dragonnettool.h"
#include "line.h"
#include "QProgressIndicator.h"

class TraceRoute : public QWidget
{
    Q_OBJECT
public:
    TraceRoute(QWidget* parent);
    ~TraceRoute();

private:
    void retranslateUi();

private:
    QGridLayout *gridLayout_5;
    QGroupBox *groupBox;
    QGridLayout *gridLayout_6;
    QFrame *frame;
    QVBoxLayout *verticalLayout;
    QLabel *label_4;
    QFrame *frame_2;
    QGridLayout *gridLayout_7;
    QLabel *label_3;
    QLineEdit *lineEdit;
    QPushButton *pushButton;
    QFrame *frame_3;
    QGridLayout *gridLayout;
    QWidget *widget;
    QGridLayout *gridLayout_2;
    QWidget *widget_4;
    QGridLayout *gridLayout_3;
    QLabel *label;
    QSpacerItem *horizontalSpacer_2;
    QSpacerItem *horizontalSpacer;
    QWidget *widget_3;
    QWidget *widget_2;
    QGridLayout *gridLayout_4;
    //QTextEdit *textBrowser;
    QTextBrowser *textBrowser;
    QSpacerItem *verticalSpacer;

    QStackedLayout* stackedLayout;
    QProgressIndicator* pi;

    QUuid uuid;

    DragonNetTool* dragon;
    QStringList *DataList;

//    QProcess* cmd;
    QString output;

private slots:
    void routeTest();
    void ViewResult();
    void linechange();
//    void readOutput();
//    void ProcessFinish(int,QProcess::ExitStatus);
    void tracefinish();

protected:
    void changeEvent(QEvent* event);
    void keyPressEvent( QKeyEvent *event );

};

#endif // TRACEROUTE_H
