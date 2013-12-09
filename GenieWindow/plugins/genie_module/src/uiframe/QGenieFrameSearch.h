#ifndef QGenieFrameSearch_H
#define QGenieFrameSearch_H
#ifndef IN_STABLE_H
#include <QObject>
#include "ui_QGenieFrameSearch.h"
#endif

class QGenieFrameSearch:public QFrame
{
    Q_OBJECT
public:
    QGenieFrameSearch(QWidget *parent=NULL);
private:
    Ui::QGenieFrameSearch mUi;
protected:
    void changeEvent(QEvent* event);
protected slots:
    void slot_openUrl();

};

#endif // QGenieFrameSearch_H
