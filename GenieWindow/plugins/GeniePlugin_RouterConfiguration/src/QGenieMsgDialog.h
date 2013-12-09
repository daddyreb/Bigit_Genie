#ifndef QGENIEDIALOGEXIT_H
#define QGENIEDIALOGEXIT_H
#ifndef IN_STABLE_H
#include <QDialog>
#endif
#include <QObject>
#include <QTimeLine>
#include "Singleton.h"
namespace Ui {
    class QGenieMsgDialog;
}

enum EXIT_RESULT{
    ER_OK,
    ER_CANCEL
};

class QGenieMsgDialog : public QDialog,public Singleton<QGenieMsgDialog>
{
    Q_OBJECT
public:
    QGenieMsgDialog(QWidget *parent = 0);
    ~QGenieMsgDialog();
    EXIT_RESULT mResult;
    void showMsg(int title,int msg,bool usetimeline=false);
    void showMsg(QString title,QString msg,bool usetimeline=false);
protected:
    void changeEvent(QEvent *e);
    void retranslateUi();
private:
    Ui::QGenieMsgDialog *ui;
protected:
#ifndef SHEET_STYLE_FORMAC
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    bool mDrag;

    QPoint mDragPosition;
#endif
    bool mbUseTimeLine;
    QTimeLine mTimeLine;
    QString mText;
    QString mTextRemain;
    int mTitle,mMsg;
protected slots:
    void on_but_ok_clicked();
    void on_but_cancel_clicked();
    void slot_timelineUpdate(int c);

};

#endif // QGENIEDIALOGEXIT_H
