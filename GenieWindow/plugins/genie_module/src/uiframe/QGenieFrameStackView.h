#ifndef QGENIEFRAMESTACKVIEW_H
#define QGENIEFRAMESTACKVIEW_H
#ifndef IN_STABLE_H
#include <QObject>
#include <QStackedWidget>
#include "Singleton.h"
#endif
class QGenieStackManager;

class QGenieFrameStackView : public QStackedWidget,public Singleton<QGenieFrameStackView>
{
    Q_OBJECT
public:
    QGenieFrameStackView(QWidget *parent);
};

#endif // QGENIEFRAMESTACKVIEW_H
