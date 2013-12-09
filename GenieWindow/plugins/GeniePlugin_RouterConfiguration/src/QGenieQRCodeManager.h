#ifndef QGENIEQRCODEMANAGER_H
#define QGENIEQRCODEMANAGER_H
#include <QtCore>
#include <QtGui>
#include <QtWidgets>
#include "Singleton.h"

class QGenieQRCodeManager:public QObject,public Singleton<QGenieQRCodeManager>
{
    Q_OBJECT
public:
    enum QRCODE_WIDGET_TYPE{
        QWT_WIRELESS
        ,QWT_GACC
        ,QWT_NUM
    };


    QGenieQRCodeManager();
    QLabel *mQRCodeW[QWT_NUM];
    void clearAll();
    void hideWidget(int type);
    void popDialog(QLabel *lab);
    QLabel *mCurLabel;
    QPushButton *but_print;
    QDialog *mDialog;
    bool mbPrintText;

    void setData(const QString &ssid,const QString &pwd,int type);
protected:
    void translateUi(QWidget *w);
    bool eventFilter(QObject *o, QEvent *e);
protected slots:
    void slot_buttonClicked();
    void slot_setPrintText(bool b);
    void slot_SetPrintButtonEnable();
};

#endif // QGENIEQRCODEMANAGER_H
