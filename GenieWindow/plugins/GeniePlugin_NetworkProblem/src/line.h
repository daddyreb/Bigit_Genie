#ifndef LINE_H
#define LINE_H

#include <QLineEdit>
#include <QEvent>
#include <QMouseEvent>

class line :public QLineEdit
{
    Q_OBJECT
public:
    line(QWidget* parent);
    ~line();

//protected:
//    void enterEvent(QEvent *event);
//    void leaveEvent(QEvent *event);
//    void mousePressEvent(QMouseEvent *event);
//    void mouseReleaseEvent(QMouseEvent *event);

//signals:
//    void finished();
};

#endif // LINE_H
