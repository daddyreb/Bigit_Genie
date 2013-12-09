#ifndef ISHELLPROVIDER_H
#define ISHELLPROVIDER_H

class IShellProvider
{
public:
	/**
	* Returns the current shell (or null if none). This return value may
	* change over time, and should not be cached.
	* 
	* @return the current shell or null if none
	*/
	virtual IShellProvider* getShell()=0;
};
#endif