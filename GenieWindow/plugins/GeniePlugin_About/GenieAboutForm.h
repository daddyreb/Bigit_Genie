#ifndef GENIEABOUTFORM_H
#define GENIEABOUTFORM_H

#include <QWidget>
#include "IManagerHandler.h"

namespace Ui {
    class GenieAboutForm;
}

class GenieAboutForm : public QWidget {
    Q_OBJECT
public:
    GenieAboutForm(QWidget *parent = 0);
    ~GenieAboutForm();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::GenieAboutForm *ui;
public:
    static IManagerHandler *mHandler;
};

#endif // GENIEABOUTFORM_H
