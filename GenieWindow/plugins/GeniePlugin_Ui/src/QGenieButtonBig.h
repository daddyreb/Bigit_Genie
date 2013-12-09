#ifndef QGENIEBUTTONBIG_H
#define QGENIEBUTTONBIG_H
//#include "QGenieButtonStackViewController.h"
#include <QVector>
#include <QtGui>
#include <QtWidgets>
#include "ui_QGenieButtonBig.h"


class QGenieButtonBig : public QPushButton
{
    Q_OBJECT
public:
    enum UPDATE_FLAG
    {
        UF_TEXT=(1<<0)
        ,UF_BUTLINE=(1<<1)
        ,UF_ICO=(1<<2)
        ,UF_ALL=0xFFFFFFFF
    };


    QGenieButtonBig(QWidget *parent=NULL);
    void setText(const QString &text);
    void updateUi(quint32 flag=UF_ALL);
protected:
    Ui::QGenieButtonBig mUi;


protected:
    friend class QGenieBigButtonManager;
private slots:

};

#endif // QGENIEBUTTONBIG_H
