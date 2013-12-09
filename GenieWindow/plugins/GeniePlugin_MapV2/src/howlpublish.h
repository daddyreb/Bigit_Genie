#ifndef HOWLPUBLISH_H
#define HOWLPUBLISH_H

#include <QThread>
#include "howl.h"

class howlpublish : public QThread
{
    Q_OBJECT
public:
    explicit howlpublish(QObject *parent = 0);
    void run();
    bool cancelPublish();

signals:
	void rebootService();
private:
    sw_discovery discovery;
    sw_discovery_publish_id	id;
public slots:
    void onSrvStarted(bool bSrvStarted);
	void onRebootService();
private:
    bool m_bSrvStarted;
    bool m_bPublished;

};

#endif // HOWLPUBLISH_H
