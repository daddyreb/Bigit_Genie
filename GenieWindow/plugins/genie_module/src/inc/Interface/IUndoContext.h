#ifndef IUNDOCONTEXT_H
#define IUNDOCONTEXT_H
#include <QString>

class IUndoContext
{
public:
	/**
	* Get the label that describes the undo context.
	* 
	* @return the label for the context.
	*/
	virtual QString getLabel()=0;

	/**
	* Return whether the specified context is considered a match for the
	* receiving context. When a context matches another context, operations
	* that have the context are considered to also have the matching context.
	* 
	* @param context 
	*            the context to be checked against the receiving context.
	* 
	* @return <code>true</code> if the receiving context can be considered a
	*         match for the specified context, and <code>false</code> if it
	*         cannot.
	*/
	virtual bool matches(IUndoContext* context)=0;
};
#endif