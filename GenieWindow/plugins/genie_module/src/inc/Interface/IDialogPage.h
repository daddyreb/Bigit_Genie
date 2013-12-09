#ifndef IDIALOGPAGE_H
#define IDIALOGPAGE_H
#include <QString>
#include <QImage>

class IDialogPage
{
public:
	/**
	* Disposes the SWT resources allocated by this
	* dialog page.
	*/
	virtual void dispose()=0;

	/**
	* Returns this dialog page's description text.
	*
	* @return the description text for this dialog page, 
	*  or <code>null</code> if none
	*/
	virtual QString getDescription()=0;

	/**
	* Returns the current error message for this dialog page.
	* May be <code>null</code> to indicate no error message.
	* <p>
	* An error message should describe some error state,
	* as opposed to a message which may simply provide instruction
	* or information to the user.
	* </p>
	* 
	* @return the error message, or <code>null</code> if none
	*/
	virtual QString getErrorMessage()=0;

	/**
	* Returns this dialog page's image.
	*
	* @return the image for this dialog page, or <code>null</code>
	*  if none
	*/
	virtual QImage getImage()=0;

	/**
	* Returns the current message for this wizard page.
	* <p>
	* A message provides instruction or information to the 
	* user, as opposed to an error message which should 
	* describe some error state.
	* </p>
	* 
	* @return the message, or <code>null</code> if none
	*/
	virtual QString getMessage()=0;

	/**
	* Returns this dialog page's title.
	*
	* @return the title of this dialog page, 
	*  or <code>null</code> if none
	*/
	virtual QString getTitle()=0;

	/**
	* Notifies that help has been requested for this dialog page.
	*/
	virtual void performHelp()=0;

	/**
	* Sets this dialog page's description text.
	* 
	* @param description the description text for this dialog
	*  page, or <code>null</code> if none
	*/
	virtual void setDescription(QString description)=0;

	/**
	* Set this dialog page's title.
	*
	* @param title the title of this dialog page, 
	*  or <code>null</code> if none
	*/
	virtual void setTitle(QString title)=0;

	/**
	* Sets the visibility of this dialog page.
	*
	* @param visible <code>true</code> to make this page visible,
	*  and <code>false</code> to hide it
	*/
	virtual void setVisible(bool visible)=0;
};
#endif