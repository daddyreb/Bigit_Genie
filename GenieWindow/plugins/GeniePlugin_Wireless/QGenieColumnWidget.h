#ifndef QGENIECOLUMNWIDGET_H
#define QGENIECOLUMNWIDGET_H

#include <QWidget>
#include <QPaintEvent>
#include <QColor>
#include <QSpacerItem>
#include <QEvent>
#include <QStringList>

class QGenieColumnUnit;

class QGenieColumnWidget : public QWidget
{
    friend class QGenieColumnUnit;
Q_OBJECT
public:
    explicit QGenieColumnWidget(QWidget *parent = 0);
protected:
    int             m_column_height;
    bool            m_bdrawTwoParts;
    int             m_itotalParts;
protected:
    virtual void paintEvent(QPaintEvent *event);
public:
    virtual QSize sizeHint() const;
};

#endif // QGENIECOLUMNWIDGET_H
