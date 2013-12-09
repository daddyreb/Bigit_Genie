#ifndef QGenieFrameAD_H
#define QGenieFrameAD_H
#ifndef IN_STABLE_H
#include <QObject>
#include "ui_QGenieFrameAD.h"
#endif

class QGenieFrameAD:public QFrame,public Singleton<QGenieFrameAD>
{
    Q_OBJECT
public:
    QGenieFrameAD(QWidget *parent=NULL);
 //   void changePage(QString s);
protected:
    void changeEvent(QEvent* event);
private:
    Ui::QGenieFrameAD mUi;

};

#endif // QGenieFrameAD_H
