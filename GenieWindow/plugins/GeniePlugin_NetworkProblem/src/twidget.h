#ifndef TWIDGET_H
#define TWIDGET_H

#include <QWidget>
#include <QEvent>
#include <QMouseEvent>

class TWidget : public QWidget
{
    Q_OBJECT
public:
    TWidget(QWidget* parent,QString& filepath);
    ~TWidget();

private:
    QString path;

private:
    void enterEvent(QEvent *e);
    void leaveEvent(QEvent *e);

signals:
    void widgetClick();

private:
    void resizeEvent(QResizeEvent *e);
};

#endif // TWIDGET_H
