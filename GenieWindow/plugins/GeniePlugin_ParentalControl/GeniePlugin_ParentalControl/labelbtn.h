#ifndef LABELBTN_H
#define LABELBTN_H

#include <QLabel>
#include <QMouseEvent>
#include <QResizeEvent>

class LabelBtn : public QLabel
{
    Q_OBJECT
public:
    LabelBtn(QWidget *parent);

signals:
    void labelClicked();
protected:
    void mousePressEvent(QMouseEvent *ev);
};

#endif // LABELBTN_H
