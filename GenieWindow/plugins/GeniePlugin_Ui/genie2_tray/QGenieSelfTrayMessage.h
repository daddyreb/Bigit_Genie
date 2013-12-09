#ifndef QGENIESELFTRAYMESSAGE_H
#define QGENIESELFTRAYMESSAGE_H


#include <QObject>
#include <QtGui>
#include <QtWidgets>
namespace Ui {
    class QGenieSelfTrayMessage;
}

class QGenieSelfTrayMessage : public QWidget {
    Q_OBJECT
public:
    QGenieSelfTrayMessage(QWidget *parent = 0);
    ~QGenieSelfTrayMessage();
    void setPosition(int x,int y);
    void showText(const QString &txt,int secs=5);

protected:
    virtual void	mousePressEvent ( QMouseEvent * event );
    QTimer mTimer;
private:
    Ui::QGenieSelfTrayMessage *ui;
    friend class QGenieTrayIco_In;

};

#endif // QGENIESELFTRAYMESSAGE_H
