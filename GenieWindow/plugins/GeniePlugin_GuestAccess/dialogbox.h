#ifndef AGAINMESSAGEBOX_H
#define AGAINMESSAGEBOX_H

#include <QtGui>
#include <QEvent>
#include "inc/genie_interface/ILanguageManager.h"

class AgainMessageBox : public QDialog
{
    Q_OBJECT
public:
    AgainMessageBox(QString& message,QString& title);
    ~AgainMessageBox();

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
    QPushButton *pushButton_2;

    QPoint last,pos0;
    QUuid uuid;
    QString boxStatus;
    int clock;

private:
    void resizeEvent(QResizeEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);

public:
    ILanguageManager *lanmanager;
    void labelchange(QString,QString);

private slots:
    void thisclose();
    void DialogClose();

signals:
    void ChangePage();
    void StopthisPage();

protected:
    void changeEvent(QEvent* event);
};

#endif // AGAINMESSAGEBOX_H
