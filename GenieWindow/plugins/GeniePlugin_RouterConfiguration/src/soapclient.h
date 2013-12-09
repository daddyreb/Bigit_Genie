#ifndef SOAP_CLIENT_H_
#define SOAP_CLIENT_H_
#include "qtsoap.h"
class ISoapClient{
public:
    virtual void parseSoapResponse(int cmd,QtSoapMessage &msg)=0;
    virtual void whenSoapError(int cmd,const QString& reson )=0;
};

#endif
