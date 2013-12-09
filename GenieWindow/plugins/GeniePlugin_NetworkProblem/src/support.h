#ifndef SUPPORT_H
#define SUPPORT_H

#include <QtGui>
#include <QtWidgets>
#include <QLineEdit>
#include "pushbutton.h"
#include "genie2_interface.h"

class Support : public QWidget
{
    Q_OBJECT
public:
    Support(QWidget* parent);
    ~Support();

private:
    void retranslateUi();

private:
    QGridLayout *gridLayout;
    QWidget *widget;
    QGridLayout *gridLayout_3;
    QSpacerItem *verticalSpacer;
    QLabel *label;
    QWidget *widget_2;
    QGridLayout *gridLayout_5;
    QFrame *frame;
    QGridLayout *gridLayout_4;
    QSpacerItem *verticalSpacer_2;
    PushButton *pushButton;
    PushButton *pushButton_2;
    QSpacerItem *horizontalSpacer;
    PushButton *pushButton_3;
    QSpacerItem *horizontalSpacer_2;
    PushButton *pushButton_4;
    QSpacerItem *verticalSpacer_3;
    QFrame *frame_2;
    QGridLayout *gridLayout_2;
    QSpacerItem *verticalSpacer_5;
    QSpacerItem *horizontalSpacer_4;
    QSpacerItem *horizontalSpacer_3;
    QSpacerItem *verticalSpacer_4;
    QLabel *label_2;
    QLabel *label_3;
    QLabel *label_4;
    QLabel *label_5;
    QLineEdit *lineEdit;
    QLineEdit *lineEdit_2;
    QLineEdit *lineEdit_3;
    QLineEdit *lineEdit_4;
    QCheckBox *checkBox;

    QPushButton *pushButton_5;
    QPushButton *pushButton_6;
    QPushButton *pushButton_7;
    QPushButton *pushButton_8;

    QUuid uuid;
    QString mOper;

private slots:
    void Downloads();
    void Forums();
    void Register();
    void Premium();
    void changePage();
    void copydownload();
    void copyforums();
    void copyregister();
    void copysupport();

protected:
    void changeEvent(QEvent* event);
};

#endif // SUPPORT_H
