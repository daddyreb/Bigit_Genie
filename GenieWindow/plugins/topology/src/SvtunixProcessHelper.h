#ifndef SVTUNIXPROCESSHELPER_H
#define SVTUNIXPROCESSHELPER_H

class SVT_UnixProcessHelper
{
public:
    SVT_UnixProcessHelper(bool *exitmsg=NULL);
    bool doCmd(const QString &cmd,QStringList &retlst);
protected:
    bool *mExitMsg;

};

#endif // SVTUNIXPROCESSHELPER_H
