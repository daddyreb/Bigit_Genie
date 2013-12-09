#ifndef QGENIESPLASH_H
#define QGENIESPLASH_H
#ifndef IN_STABLE_H
#include <QObject>
#include <QWidget>
#endif

namespace Ui {
    class QGenieSplash;
}

class QGenieSplash : public QWidget {
    Q_OBJECT
public:
    QGenieSplash(QWidget *parent = 0);
    ~QGenieSplash();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::QGenieSplash *ui;
};

#endif // QGENIESPLASH_H
