#ifndef VERIFYDAEMONHANDLER_H
#define VERIFYDAEMONHANDLER_H

class VerifyDaemonHandler
{
public:
    VerifyDaemonHandler();

public:
    static bool doVerify();
private:
    static bool tryStartDaemon();
};

#endif // VERIFYDAEMONHANDLER_H
