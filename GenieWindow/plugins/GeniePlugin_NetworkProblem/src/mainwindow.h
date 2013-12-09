#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include "twidget.h"
#include "support.h"
#include "faq.h"
#include "tools.h"
#include "language_NetworkProblem.h"

class MainWindow : public QWidget
{
    Q_OBJECT
public:
    MainWindow();
    ~MainWindow();

private:
    void retranslateUi();

private:
    QGridLayout *gridLayout;
    QTabWidget *tabWidget;
    QWidget *tab;
    QWidget *tab_2;
    QWidget *tab_3;
    QStackedLayout* stackedLayout;
    QStackedLayout* stackedLayout_2;
    QStackedLayout* stackedLayout_3;
    Support * sp;
    FAQ* faq;
    Tools* tools;

    QUuid uuid;

signals:
    void getprofile();

protected:
    void changeEvent(QEvent* event);
protected slots:
    void slot_ClearDirtyFocusFrame();
};

#endif // MAINWINDOW_H
