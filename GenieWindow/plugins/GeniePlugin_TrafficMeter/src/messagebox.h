#ifndef MESSAGEBOX_H
#define MESSAGEBOX_H

#include <QtGui>
#include <QEvent>
#include "inc/genie_interface/ILanguageManager.h"

class MessageBox : public QDialog
{
    Q_OBJECT
public:
    MessageBox(QString& message,QString& title);
    ~MessageBox();

private:
    void retranslateUi();

private:
    QGridLayout *gridLayout;
    QFrame *frame;
    QGridLayout *gridLayout_2;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QSpacerItem *horizontalSpacer;
    QPushButton *pushButton;
    QSpacerItem *horizontalSpacer_2;

    QPoint last,pos0;
    QUuid uuid;

private:
    void resizeEvent(QResizeEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);

public:
    ILanguageManager *lanmanager;
    void labelchange(QString,QString);
//    void changwidget();

//signals:
//    void changePage();

protected:
    void changeEvent(QEvent* event);
};

#endif // MESSAGEBOX_H
