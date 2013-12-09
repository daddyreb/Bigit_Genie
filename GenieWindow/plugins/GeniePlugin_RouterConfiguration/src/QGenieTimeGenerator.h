#ifndef QGENIETIMEGENERATOR_H
#define QGENIETIMEGENERATOR_H

#include <QString>
#define SMARTNETWORK_WAIT_TIME 40
class QGenieTimeGenerator
{
public:
    QGenieTimeGenerator();
    int gaccSettingWaitTime(const QString &router);
    int wirelessSettingWaitTime(const QString &router);
};

#endif // QGENIETIMEGENERATOR_H
