#ifndef PING_H
#define PING_H

#include <QtGui>
#include <QtWidgets>
#include "../../DragonNetTool/interface/dragonnettool.h"
#include "QProgressIndicator.h"
#include "line.h"
#include "QProgressIndicator.h"

class MyThread : public QThread
{
    Q_OBJECT
public:
    MyThread(QObject *parent=0);
    ~MyThread();
protected:
     void run();

private:
    DragonNetTool dragon;
    QStringList **DataList;
    QString       m_strhost;

public:
    void begin_ping(const QString &host,QStringList **list);

signals:
    void PingFinish();
public slots:
    void on_PingFinish();
};

class Ping : public QWidget
{
    Q_OBJECT
public:
    Ping(QWidget* parent);
    ~Ping();

private:
    void retranslateUi();

private:
    QGridLayout *gridLayout_5;
    QGroupBox *groupBox;
    QGridLayout *gridLayout_6;
    QFrame *frame;
    QVBoxLayout *verticalLayout;
    QLabel *label_4;
    QLabel *label_2;
    QFrame *frame_2;
    QGridLayout *gridLayout_7;
    QLabel *label_3;
    QLineEdit *lineEdit;
    QPushButton *pushButton;
    QFrame *frame_3;
    QGridLayout *gridLayout;
    QWidget *widget;
    QGridLayout *gridLayout_2;
    QWidget *widget_3;
    QWidget *widget_4;
    QGridLayout *gridLayout_3;
    QLabel *label;
    QSpacerItem *horizontalSpacer_2;
    QSpacerItem *horizontalSpacer;
    QWidget *widget_2;
    QGridLayout *gridLayout_4;
    QTextBrowser *textBrowser;
    QSpacerItem *verticalSpacer;

    QStackedLayout* stackedLayout;
    QProgressIndicator* pi;

    QUuid uuid;

    DragonNetTool* dragon;
    QStringList *DataList;

//    MyThread g_thread;

private slots:
    void pingTest();
    void ViewResult();
    void linechange();
    void pingfinished();

protected:
    void changeEvent(QEvent* event);
    void keyPressEvent( QKeyEvent *event );

};


#endif // PING_H
