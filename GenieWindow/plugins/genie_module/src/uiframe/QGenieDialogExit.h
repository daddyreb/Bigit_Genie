#ifndef QGENIEDIALOGEXIT_H
#define QGENIEDIALOGEXIT_H
#ifndef IN_STABLE_H
#include <QDialog>
#endif
#include <QObject>
namespace Ui {
    class QGenieDialogExit;
}

enum EXIT_RESULT{
    ER_OK,
    ER_CANCEL
};

class QGenieDialogExit : public QDialog {
    Q_OBJECT
public:
    QGenieDialogExit(QWidget *parent = 0);
    ~QGenieDialogExit();
    EXIT_RESULT mResult;
    void retranslateUi();
protected:
    void changeEvent(QEvent *e);

public:
    Ui::QGenieDialogExit *ui;
protected:
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    bool mDrag;
    QPoint mDragPosition;
protected slots:
    void on_but_ok_clicked();
    void on_but_cancel_clicked();

};

#endif // QGENIEDIALOGEXIT_H
