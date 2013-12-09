#ifndef LOG_H
#define LOG_H

#include "common.h"
class Log
{
public:
    static bool init (void);

    static bool report (const QString &msg);

private:
    Log(){};

    static QMutex           m_mutex;

    static QFile            m_file;

    const static QString    m_name;
};

#endif // LOG_H
