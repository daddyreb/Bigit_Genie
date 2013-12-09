#ifndef QGENIEREBOOTOBJECT_H
#define QGENIEREBOOTOBJECT_H
#include <QObject>
#include <QtGui>
#include <QtWidgets>
#include "Singleton.h"
class QGenieRebootObject : public QObject,public Singleton<QGenieRebootObject>
{
	Q_OBJECT
public:
    explicit QGenieRebootObject(QObject *parent = 0);
    ~QGenieRebootObject();
    QWidget * getRebootUi();
	void retranslateUi();
protected:
    QWidget *mW;
	int mSession;

public slots:
	void slot_buttonClicked();
	void slot_RebootFinish();

    friend class QGenieRCWirelessObject;
};

#endif // QGENIEREBOOTOBJECT_H
