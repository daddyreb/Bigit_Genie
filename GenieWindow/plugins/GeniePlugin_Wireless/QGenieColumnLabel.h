#ifndef QGENIECOLUMNLABEL_H
#define QGENIECOLUMNLABEL_H

#include <QLabel>
#include <QMouseEvent>

class QGenieColumnLabel : public QLabel
{
Q_OBJECT
public:
    explicit QGenieColumnLabel(QWidget *parent = 0);

protected:
    virtual  void   mouseReleaseEvent ( QMouseEvent * event );
signals:
    void clicked();
public slots:

};

#endif // QGENIECOLUMNLABEL_H
