#ifndef QGENIESELFTRAYMESSAGE_H
#define QGENIESELFTRAYMESSAGE_H

#ifndef IN_STABLE_H
#include <QObject>
#include <QtGui>
#endif
#ifdef SELF_TRAY_BOX
namespace Ui {
    class QGenieSelfTrayMessage;
}

class QGenieSelfTrayMessage : public QWidget {
    Q_OBJECT
public:
    QGenieSelfTrayMessage(QWidget *parent = 0);
    ~QGenieSelfTrayMessage();
    void setPosition(int x,int y);

protected:
    void changeEvent(QEvent *e);
    virtual void	mousePressEvent ( QMouseEvent * event );

private:
    Ui::QGenieSelfTrayMessage *ui;
    friend class QGenieTrayIco_In;

};
#endif

#endif // QGENIESELFTRAYMESSAGE_H
