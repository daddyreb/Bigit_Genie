#ifndef FAQ_H
#define FAQ_H

#include <QtGui>
#include "flex.h"
#include "twidget.h"

class FAQ : public QWidget
{
public:
    FAQ(QWidget* parent);
    ~FAQ();

private:
    void retranslateUi();

private:
    QVBoxLayout *verticalLayout;
    QWidget *widget;
    QVBoxLayout *verticalLayout_2;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QVBoxLayout *verticalLayout_3;
    QFrame *frame_8;
    QGridLayout *gridLayout;
    QGridLayout *gridLayout_3;
    QWidget *widget_2;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QFrame *line;
    Flex *frame;
    Flex *frame_3;
    Flex *frame_2;
    Flex *frame_4;
    Flex *frame_5;
    Flex *frame_6;
    Flex *frame_7;
    Flex *frame_9;
    TWidget* twidget;

    QUuid uuid;

private:
    QString str,str_1;
    QString str1,str1_1;
    QString str2,str2_1;
    QString str3,str3_1;
    QString str4,str4_1;
    QString str5,str5_1;
    QString str6,str6_1;
    QString str7,str7_1;

protected:
    void changeEvent(QEvent* event);
};

#endif // FAQ_H
