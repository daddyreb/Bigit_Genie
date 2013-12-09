#ifndef QGENIECOLUMNUNIT_H
#define QGENIECOLUMNUNIT_H

#include <QWidget>
#include "QGenieColumnLabel.h"

#include "QGenieColumnWidget.h"

class QGenieColumnUnit : public QWidget
{
    Q_OBJECT
public:
    explicit QGenieColumnUnit(QWidget *parent = 0);

protected:
    QGenieColumnLabel       *m_column_lb;
    QGenieColumnWidget      *m_column_widget;
public:
    int getColumnBlockHeight();
    void setLabelText(const QString &str_lable);
    void setColumnTooltip(const QString &tooltip);
    void setColumnBlockHeight(int height);
    void updateColumnBlock(bool btwoParts,int totalParts);

};

#endif // QGENIECOLUMNUNIT_H
