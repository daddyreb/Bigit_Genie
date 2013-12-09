#ifndef QGENIEPROGRESSPANEL_H
#define QGENIEPROGRESSPANEL_H

#include <QWidget>
#include "QProgressIndicator.h"


class QGenieProgressPanel : public QWidget
{
Q_OBJECT
public:
    explicit QGenieProgressPanel(QWidget *parent = 0);
protected:
    QProgressIndicator          *m_progress_indicator;
public:
    void start();
    void stop();
    void setAnimationDelay(int delay);
public:
    QSize sizeHint() const;
signals:

public slots:

};

#endif // QGENIEPROGRESSPANEL_H
