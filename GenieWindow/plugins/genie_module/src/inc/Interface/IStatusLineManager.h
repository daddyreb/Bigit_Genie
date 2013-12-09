#ifndef ISTATUSLINEMANAGER_H
#define ISTATUSLINEMANAGER_H
#include <QImage>
#include <QString>
class IProgressMonitor;

class IStatusLineManager
{
public:
	/**
	* Returns a progress monitor which reports progress in the status line.
	* Note that the returned progress monitor may only be accessed from the UI
	* thread.
	* 
	* @return the progress monitor
	* 
	*         Note: There is a delay after a beginTask message before the
	*         monitor is shown. This may not be appropriate for all apps.
	*/
	virtual IProgressMonitor* getProgressMonitor()=0;

	/**
	* Returns whether the cancel button on the status line's progress monitor
	* is enabled.
	*
	* @return <code>true</code> if the cancel button is enabled, or <code>false</code> if not
	*/
	virtual bool isCancelEnabled()=0;

	/**
	* Sets whether the cancel button on the status line's progress monitor
	* is enabled.
	*
	* @param enabled <code>true</code> if the cancel button is enabled, or <code>false</code> if not
	*/
	virtual void setCancelEnabled(bool enabled)=0;

	/**
	* Sets the error message text to be displayed on the status line.
	* The image on the status line is cleared.
	* <p>
	* An error message overrides the current message until the error 
	* message is cleared (set to <code>null</code>).
	* </p>
	*
	* @param message the error message, or <code>null</code> to clear
	* 		the current error message.
	*/
	virtual void setErrorMessage(QString message)=0;

	/**
	* Sets the image and error message to be displayed on the status line.
	* <p>
	* An error message overrides the current message until the error 
	* message is cleared (set to <code>null</code>).
	* </p>
	*
	* @param image the image to use, or <code>null</code> for no image
	* @param message the error message, or <code>null</code> to clear
	* 		the current error message.
	*/
	virtual void setErrorMessage(QImage image, QString message)=0;

	/**
	* Sets the message text to be displayed on the status line.
	* The image on the status line is cleared.
	* <p>
	* This method replaces the current message but does not affect the 
	* error message. That is, the error message, if set, will continue
	* to be displayed until it is cleared (set to <code>null</code>).
	* </p>
	*
	* @param message the message, or <code>null</code> for no message
	*/
	virtual void setMessage(QString message)=0;

	/**
	* Sets the image and message to be displayed on the status line.
	* <p>
	* This method replaces the current message but does not affect the 
	* error message. That is, the error message, if set, will continue
	* to be displayed until it is cleared (set to <code>null</code>).
	* </p>
	*
	* @param image the image to use, or <code>null</code> for no image
	* @param message the message, or <code>null</code> for no message
	*/
	virtual void setMessage(QImage image, QString message)=0;
};
#endif