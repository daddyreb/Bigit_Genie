#ifndef QGENIELISTWIDGET_H
#define QGENIELISTWIDGET_H

#include <QListWidget>

class QGenieListWidget : public QListWidget
{
    Q_OBJECT
public:
    explicit QGenieListWidget(QWidget *parent = 0);
    
signals:
    
public slots:
    friend class QGenieWiFiProfileWidget;
    
};

#endif // QGENIELISTWIDGET_H
