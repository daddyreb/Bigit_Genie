#ifndef ISAVEABLEFILTER_H
#define ISAVEABLEFILTER_H
class IWorkbenchPart;
class Saveable;
class ISaveableFilter
{
public:
	/**
	* Indicate whether the given saveable matches this filter.
	* @param saveable the saveable being tested
	* @param containingParts the parts that contain the saveable. This list may 
	*    contain zero or more parts.
	* @return whether the given saveable matches this filter
	*/
	virtual bool select(Saveable* saveable, IWorkbenchPart* containingParts)=0;
};
#endif