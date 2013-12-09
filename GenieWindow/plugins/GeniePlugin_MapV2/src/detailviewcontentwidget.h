#ifndef DETAILVIEWCONTENTWIDGET_H
#define DETAILVIEWCONTENTWIDGET_H

#include <QWidget>

class QPaintEvent;

class DetailViewContentWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DetailViewContentWidget(QWidget *parent = 0);

signals:

public slots:

protected:
    void paintEvent(QPaintEvent *event);
};

#endif // DETAILVIEWCONTENTWIDGET_H
