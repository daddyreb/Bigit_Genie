#ifndef PUSHBUTTON_H
#define PUSHBUTTON_H

#include <QPushButton>
#include <QKeyEvent>
#include <QEvent>

class PushButton : public QPushButton
{
    Q_OBJECT
public:
    PushButton(QWidget* parent);
    ~PushButton();

private:
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);
};

#endif // PUSHBUTTON_H
