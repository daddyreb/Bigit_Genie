#ifndef TEXTLABEL_H
#define TEXTLABEL_H

#include <QLabel>
#include <QMouseEvent>

class TextLabel : public QLabel
{
    Q_OBJECT
public:
    TextLabel(QWidget *parent);

signals:
    void textLabelClicked();

protected:
    void mousePressEvent(QMouseEvent *ev);
};

#endif // TEXTLABEL_H
