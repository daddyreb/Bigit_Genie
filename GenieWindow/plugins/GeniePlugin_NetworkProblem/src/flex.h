#ifndef FLEX_H
#define FLEX_H

#include <QtGui>
#include <QtWidgets>
#include "twidget.h"
#include "label.h"
#include "pushbutton.h"

class Flex : public QWidget
{
    Q_OBJECT
public:
    Flex(const QString &lbl1,const QString &lbl2, const QString &mark, QWidget* parent);
    ~Flex();

private:
    QGridLayout *gridLayout_3;
    QFrame *frame;
    QGridLayout *gridLayout;
    Flexlabel *label;
    PushButton *pushButton;
    QFrame *line;
    QFrame *frame_2;
    QGridLayout *gridLayout_2;
    QLabel *label_2;
    QLabel *label_3;
    QLabel* label_4;
    QLabel *label_space;
    TWidget* twidget;

    QString Mark;

public:
    void labelchange(QString,QString);

private slots:
    void btnchang();
    void openUrl(QString);
    void labelflex();

};

#endif // FLEX_H
