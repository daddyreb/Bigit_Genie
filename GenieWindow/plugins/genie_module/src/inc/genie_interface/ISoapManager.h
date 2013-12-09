#ifndef ISOAPMANAGER_H
#define ISOAPMANAGER_H
class NetgearSoapSchedule;

class ISoapManager
{
  //  public virtual xxxx()=0;
public:
    virtual NetgearSoapSchedule* getSoapAPI()=0;
};

#endif // ISOAPMANAGER_H
