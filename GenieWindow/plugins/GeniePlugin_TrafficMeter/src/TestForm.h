#ifndef TESTFORM_H
#define TESTFORM_H

#include <QWidget>
#include "IManagerHandler.h"

namespace Ui {
    class TestForm;
}

class TestForm : public QWidget {
    Q_OBJECT
public:
    TestForm(QWidget *parent = 0);
    ~TestForm();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::TestForm *ui;
public:
    static IManagerHandler *sHandler;
protected slots:
    void on_but_1_clicked();
    void on_but_2_clicked();
};

#endif // TESTFORM_H
