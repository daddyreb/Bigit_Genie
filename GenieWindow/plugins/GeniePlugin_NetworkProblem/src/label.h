#ifndef LABEL_H
#define LABEL_H

#include <QLabel>
#include <QMouseEvent>

class Flexlabel : public QLabel
{
    Q_OBJECT
public:
    Flexlabel(QWidget* parent);
    ~Flexlabel();

private:
    void mousePressEvent(QMouseEvent *event);
    void leaveEvent(QEvent *event);
    void enterEvent(QEvent *event);

signals:
    void labelclick();
};

#endif // LABEL_H
