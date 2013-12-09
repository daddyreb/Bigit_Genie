#ifndef DNSLOOKUP_H
#define DNSLOOKUP_H

#include <QtGui>
#include <QtWidgets>
#include "../../DragonNetTool/interface/dragonnettool.h"
#include "line.h"
#include "QProgressIndicator.h"

class DNSLookup : public QWidget
{
    Q_OBJECT
public:
    DNSLookup(QWidget* parent);
    ~DNSLookup();

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

private slots:
    void dnslookupTest();
    void ViewResult();
    void linechange();

protected:
    void changeEvent(QEvent* event);
    void keyPressEvent( QKeyEvent *event );

};

#endif // DNSLOOKUP_H
