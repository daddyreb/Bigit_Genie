#ifndef IWIZARDDESCRIPTOR_H
#define IWIZARDDESCRIPTOR_H
#include "IWorkbenchPartDescriptor.h"
#include "IAdaptable.h"
#include <QString>

class IStructuredSelection;
class IWizardCategory;

class IWizardDescriptor:public IWorkbenchPartDescriptor, public IAdaptable
{
public:
	/**
	* Answer the selection for the reciever based on whether the it can handle
	* the selection. If it can return the selection. If it can handle the
	* adapted to IResource value of the selection. If it satisfies neither of
	* these conditions return an empty IStructuredSelection.
	* 
	* @return IStructuredSelection
	* @param selection
	*            IStructuredSelection
	*/
	IStructuredSelection* adaptedSelection(IStructuredSelection* selection)=0;

	/**
	* Return the description.
	* 
	* @return the description
	*/
	QString getDescription()=0;

	/**
	* Return the tags associated with this wizard.
	* 
	* @return the tags associated with this wizard
	*/
	QString* getTags()=0;


	/**
	* Return the help system href for this wizard.
	* 
	* @return the help system href for this wizard or <code>null</code>
	*/
	QString getHelpHref()=0;	

	/**
	* Return the category for this wizard.  
	* 
	* @return the category or <code>null</code>
	*/
	IWizardCategory* getCategory()=0;

	/**
	* Answer <code>true</code> if this wizard is able to finish without
	* loading any pages. This is a hint to any
	* {@link org.eclipse.jface.wizard.WizardSelectionPage} or container that
	* may contain this wizard to allow the finish button to be pressed without
	* actually entering the wizard. If this occurs the
	* {@link org.eclipse.jface.wizard.IWizard#performFinish()} method should be
	* invoked by the containing wizard without creating any pages.
	* 
	* @return <code>true</code> if this wizard can finish immediately
	*/
	bool canFinishEarly()=0;

	/**
	* Answer <code>true</code> if this wizard has any pages. This is a
	* hint to any {@link org.eclipse.jface.wizard.WizardSelectionPage} or
	* container that may contain this wizard that they should enable the "Next"
	* button, if appropriate.
	* 
	* @return <code>true</code> if this wizard has wizard pages
	*/
	bool hasPages()=0;
};
#endif