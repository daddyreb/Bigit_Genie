#ifndef PROGRESSPANEL_H
#define PROGRESSPANEL_H

#include <QWidget>
#include "myprogressindicator.h"

class ProgressWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ProgressWidget(QWidget *parent = 0);

public:
    void start();
    void stop();
    void setAnimationDelay(int delay);

    QSize sizeHint() const;
    
signals:
    
public slots:

protected:
    MyProgressIndicator *m_progress_indicator;
    
};

#endif // PROGRESSPANEL_H
