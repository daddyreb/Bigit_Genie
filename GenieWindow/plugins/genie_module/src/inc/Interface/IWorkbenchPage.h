#ifndef IWORKBENCHPAGE_H
#define IWORKBENCHPAGE_H
#include "IPartService.h"
#include "ISelectionService.h"
#include <QStringList>
#include <QString>

class IWorkbenchPart;
class IPropertyChangeListener;
class IEditorReference;
class IEditorPart;
class IAdaptable;
class IPerspectiveDescriptor;
class IViewPart;
class IViewReference;
class IWorkingSet;
class IEditorInput;
class IWorkbenchWindow;

class IWorkbenchPage:public IPartService,public ISelectionService
{
public:
	/**
	* Activates the given part. The part will be brought to the front and given
	* focus. The part must belong to this page.
	* 
	* @param part
	*            the part to activate
	*/
	virtual void activate(IWorkbenchPart* part)=0;

	/**
	* Adds a property change listener.
	* 
	* @param listener
	*            the property change listener to add
	* @since 2.0
	* @deprecated client should register listeners on the instance of
	*             {@link org.eclipse.ui.IWorkingSetManager} returned by
	*             {@link org.eclipse.ui.IWorkbench#getWorkingSetManager()}
	*             instead.
	*/
	virtual void addPropertyChangeListener(IPropertyChangeListener* listener)=0;

	/**
	* Moves the given part forward in the Z order of this page so as to make it
	* visible, without changing which part has focus. The part must belong to
	* this page.
	* 
	* @param part
	*            the part to bring forward
	*/
	virtual void bringToTop(IWorkbenchPart* part)=0;

	/**
	* Closes this workbench page. If this page is the active one, this honor is
	* passed along to one of the window's other pages if possible.
	* <p>
	* If the page has an open editor with unsaved content, the user will be
	* given the opportunity to save it.
	* </p>
	* 
	* @return <code>true</code> if the page was successfully closed, and
	*         <code>false</code> if it is still open
	*/
	virtual bool close()=0;

	/**
	* Closes all of the editors belonging to this workbench page.
	* <p>
	* If the page has open editors with unsaved content and <code>save</code>
	* is <code>true</code>, the user will be given the opportunity to save
	* them.
	* </p>
	* 
	* @param save
	* 
	* @return <code>true</code> if all editors were successfully closed, and
	*         <code>false</code> if at least one is still open
	*/
	virtual bool closeAllEditors(bool save)=0;

	/**
	* Closes the given <code>Array</code> of editor references. The editors
	* must belong to this workbench page.
	* <p>
	* If any of the editors have unsaved content and <code>save</code> is
	* <code>true</code>, the user will be given the opportunity to save
	* them.
	* </p>
	* 
	* @param editorRefs
	*            the editors to close
	* @param save
	*            <code>true</code> to save the editor contents if required
	*            (recommended), and <code>false</code> to discard any unsaved
	*            changes
	* @return <code>true</code> if the editors were successfully closed, and
	*         <code>false</code> if the editors are still open
	* @since 3.0
	*/
	virtual bool closeEditors(IEditorReference* editorRefs, bool save)=0;

	/**
	* Closes the given editor. The editor must belong to this workbench page.
	* <p>
	* If the editor has unsaved content and <code>save</code> is
	* <code>true</code>, the user will be given the opportunity to save it.
	* </p>
	* 
	* @param editor
	*            the editor to close
	* @param save
	*            <code>true</code> to save the editor contents if required
	*            (recommended), and <code>false</code> to discard any unsaved
	*            changes
	* @return <code>true</code> if the editor was successfully closed, and
	*         <code>false</code> if the editor is still open
	*/
	virtual bool closeEditor(IEditorPart* editor, bool save)=0;

	/**
	* Returns the view in this page with the specified id. There is at most one
	* view in the page with the specified id.
	* 
	* @param viewId
	*            the id of the view extension to use
	* @return the view, or <code>null</code> if none is found
	*/
	virtual IViewPart* findView(QString viewId)=0;

	/**
	* Returns the view reference with the specified id.
	* 
	* @param viewId
	*            the id of the view extension to use
	* @return the view reference, or <code>null</code> if none is found
	* @since 3.0
	*/
	virtual IViewReference* findViewReference(QString viewId)=0;

	/**
	* Returns the view reference with the specified id and secondary id.
	* 
	* @param viewId
	*            the id of the view extension to use
	* @param secondaryId
	*            the secondary id to use, or <code>null</code> for no
	*            secondary id
	* @return the view reference, or <code>null</code> if none is found
	* @since 3.0
	*/
	virtual IViewReference* findViewReference(QString viewId, QString secondaryId)=0;

	/**
	* Returns the active editor open in this page.
	* <p>
	* This is the visible editor on the page, or, if there is more than one
	* visible editor, this is the one most recently brought to top.
	* </p>
	* 
	* @return the active editor, or <code>null</code> if no editor is active
	*/
	virtual IEditorPart* getActiveEditor()=0;

	/**
	* Returns the editor with the specified input. Returns null if there is no
	* opened editor with that input.
	* 
	* @param input
	*            the editor input
	* @return an editor with input equals to <code>input</code>
	*/
	virtual IEditorPart* findEditor(IEditorInput* input)=0;

	/**
	* Returns an array of editor references that match the given input and/or
	* editor id, as specified by the given match flags. Returns an empty array
	* if there are no matching editors, or if matchFlags is MATCH_NONE.
	* 
	* @param input
	*            the editor input, or <code>null</code> if MATCH_INPUT is not
	*            specified in matchFlags
	* @param editorId
	*            the editor id, or <code>null</code> if MATCH_ID is not
	*            specified in matchFlags
	* @param matchFlags
	*            a bit mask consisting of zero or more of the MATCH_* constants
	*            OR-ed together
	* @return the references for the matching editors
	* 
	* @see #MATCH_NONE
	* @see #MATCH_INPUT
	* @see #MATCH_ID
	* @since 3.2
	*/
	virtual IEditorReference* findEditors(IEditorInput* input, QString editorId,
		int matchFlags)=0;

	/**
	* Returns a list of the editors open in this page.
	* <p>
	* Note that each page has its own editors; editors are never shared between
	* pages.
	* </p>
	* 
	* @return a list of open editors
	* 
	* @deprecated use #getEditorReferences() instead
	*/
	virtual IEditorPart* getEditors()=0;

	/**
	* Returns an array of references to open editors in this page.
	* <p>
	* Note that each page has its own editors; editors are never shared between
	* pages.
	* </p>
	* 
	* @return a list of open editors
	*/
	virtual IEditorReference* getEditorReferences()=0;

	/**
	* Returns a list of dirty editors in this page.
	* 
	* @return a list of dirty editors
	*/
	virtual IEditorPart* getDirtyEditors()=0;

	/**
	* Returns the input for this page.
	* 
	* @return the input for this page, or <code>null</code> if none
	*/
	virtual IAdaptable* getInput()=0;

	/**
	* Returns the page label. This will be a unique identifier within the
	* containing workbench window.
	* 
	* @return the page label
	*/
	virtual QString getLabel()=0;

	/**
	* Returns the current perspective descriptor for this page, or
	* <code>null</code> if there is no current perspective.
	* 
	* @return the current perspective descriptor or <code>null</code>
	* @see #setPerspective
	* @see #savePerspective
	*/
	virtual IPerspectiveDescriptor* getPerspective()=0;

	/**
	* Returns a list of the reference to views visible on this page.
	* <p>
	* Note that each page has its own views; views are never shared between
	* pages.
	* </p>
	* 
	* @return a list of references to visible views
	*/
	virtual IViewReference* getViewReferences()=0;

	/**
	* Returns a list of the views visible on this page.
	* <p>
	* Note that each page has its own views; views are never shared between
	* pages.
	* </p>
	* 
	* @return a list of visible views
	* 
	* @deprecated use #getViewReferences() instead.
	*/
	virtual IViewPart* getViews()=0;

	/**
	* Returns the workbench window of this page.
	* 
	* @return the workbench window
	*/
	virtual IWorkbenchWindow* getWorkbenchWindow()=0;

	/**
	* Returns the working set of this page.
	* 
	* @return the working set of this page.
	* @since 2.0
	* @deprecated individual views should store a working set if needed
	*/
	virtual IWorkingSet* getWorkingSet()=0;

	/**
	* Hides an action set in this page.
	* <p>
	* In most cases where this method is used the caller is tightly coupled to
	* a particular action set. They define it in the registry and may make it
	* visible in certain scenarios by calling <code>showActionSet</code>. A
	* static variable is often used to identify the action set id in caller
	* code.
	* </p>
	* 
	* @param actionSetID
	*            the action set to hide
	*/
	virtual void hideActionSet(QString actionSetID)=0;

	/**
	* Hides the given view. The view must belong to this page.
	* 
	* @param view
	*            the view to hide
	*/
	virtual void hideView(IViewPart* view)=0;

	/**
	* Hides the given view that belongs to the reference, if any.
	* 
	* @param view
	*            the references whos view is to be hidden
	* @since 3.0
	*/
	virtual void hideView(IViewReference* view)=0;

	/**
	* Returns whether the specified part is visible.
	* 
	* @param part
	*            the part to test
	* @return bool <code>true</code> if part is visible
	*/
	virtual bool isPartVisible(IWorkbenchPart* part)=0;

	/**
	* Returns whether the page's current perspective is showing the editor
	* area.
	* 
	* @return <code>true</code> when editor area visible, <code>false</code>
	*         otherwise
	*/
	virtual bool isEditorAreaVisible()=0;

	/**
	* Opens an editor on the given input.
	* <p>
	* If this page already has an editor open on the target input that editor
	* is activated; otherwise, a new editor is opened. Two editor inputs,
	* input1 and input2, are considered the same if
	* 
	* <pre>
	* input1.equals(input2) == true
	* </pre>.
	* </p>
	* <p>
	* The editor type is determined by mapping <code>editorId</code> to an
	* editor extension registered with the workbench. An editor id is passed
	* rather than an editor object to prevent the accidental creation of more
	* than one editor for the same input. It also guarantees a consistent
	* lifecycle for editors, regardless of whether they are created by the user
	* or restored from saved data.
	* </p>
	* 
	* @param input
	*            the editor input
	* @param editorId
	*            the id of the editor extension to use
	* @return an open and active editor, or <code>null</code> if an external
	*         editor was opened
	* @exception PartInitException
	*                if the editor could not be created or initialized
	*/
	virtual IEditorPart* openEditor(IEditorInput* input, QString editorId)=0;

	/**
	* Opens an editor on the given input.
	* <p>
	* If this page already has an editor open on the target input that editor
	* is brought to the front; otherwise, a new editor is opened. Two editor
	* inputs are considered the same if they equal. See
	* <code>Object.equals(Object)=0<code>
	* and <code>IEditorInput</code>. If <code>activate == true</code> the editor
	* will be activated.  
	* </p><p>
	* The editor type is determined by mapping <code>editorId</code> to an editor
	* extension registered with the workbench.  An editor id is passed rather than
	* an editor object to prevent the accidental creation of more than one editor
	* for the same input. It also guarantees a consistent lifecycle for editors,
	* regardless of whether they are created by the user or restored from saved 
	* data.
	* </p>
	* 
	* @param input the editor input
	* @param editorId the id of the editor extension to use
	* @param activate if <code>true</code> the editor will be activated
	* @return an open editor, or <code>null</code> if an external editor was opened
	* @exception PartInitException if the editor could not be created or initialized
	*/
	virtual IEditorPart* openEditor(IEditorInput* input, QString editorId,
		bool activate)=0 ;

	/**
	* Opens an editor on the given input.
	* <p>
	* If this page already has an editor open that matches the given input
	* and/or editor id (as specified by the matchFlags argument)=0, that editor
	* is brought to the front; otherwise, a new editor is opened. Two editor
	* inputs are considered the same if they equal. See
	* <code>Object.equals(Object)=0<code>
	* and <code>IEditorInput</code>. If <code>activate == true</code> the editor
	* will be activated.  
	* </p><p>
	* The editor type is determined by mapping <code>editorId</code> to an editor
	* extension registered with the workbench.  An editor id is passed rather than
	* an editor object to prevent the accidental creation of more than one editor
	* for the same input. It also guarantees a consistent lifecycle for editors,
	* regardless of whether they are created by the user or restored from saved 
	* data.
	* </p>
	* 
	* @param input the editor input
	* @param editorId the id of the editor extension to use
	* @param activate if <code>true</code> the editor will be activated
	* @param matchFlags a bit mask consisting of zero or more of the MATCH_* constants OR-ed together
	* @return an open editor, or <code>null</code> if an external editor was opened
	* @exception PartInitException if the editor could not be created or initialized
	* 
	* @see #MATCH_NONE
	* @see #MATCH_INPUT
	* @see #MATCH_ID
	* @since 3.2
	*/
	virtual IEditorPart* openEditor( IEditorInput* input,
		 QString editorId,  bool activate,  int matchFlags)=0;

	/**
	* Removes the property change listener.
	* 
	* @param listener
	*            the property change listener to remove
	* @since 2.0
	*/
	virtual void removePropertyChangeListener(IPropertyChangeListener* listener)=0;

	/**
	* Changes the visible views, their layout, and the visible action sets
	* within the page to match the current perspective descriptor. This is a
	* rearrangement of components and not a replacement. The contents of the
	* current perspective descriptor are unaffected.
	* <p>
	* For more information on perspective change see
	* <code>setPerspective()=0</code>.
	* </p>
	*/
	virtual void resetPerspective()=0;

	/**
	* Saves the contents of all dirty editors belonging to this workbench page.
	* If there are no dirty editors this method returns without effect.
	* <p>
	* If <code>confirm</code> is <code>true</code> the user is prompted to
	* confirm the command.
	* </p>
	* <p>
	* Note that as of 3.2, this method also saves views that implement
	* ISaveablePart and are dirty.
	* </p>
	* 
	* @param confirm <code>true</code> to ask the user before saving unsaved
	*            changes (recommended)=0, and <code>false</code> to save
	*            unsaved changes without asking
	* @return <code>true</code> if the command succeeded, and
	*         <code>false</code> if the operation was canceled by the user or
	*         an error occurred while saving
	*/
	virtual bool saveAllEditors(bool confirm)=0;

	/**
	* Saves the contents of the given editor if dirty. If not, this method
	* returns without effect.
	* <p>
	* If <code>confirm</code> is <code>true</code> the user is prompted to
	* confirm the command. Otherwise, the save happens without prompt.
	* </p>
	* <p>
	* The editor must belong to this workbench page.
	* </p>
	* 
	* @param editor
	*            the editor to close
	* @param confirm
	*            <code>true</code> to ask the user before saving unsaved
	*            changes (recommended)=0, and <code>false</code> to save
	*            unsaved changes without asking
	* @return <code>true</code> if the command succeeded, and
	*         <code>false</code> if the editor was not saved
	*/
	virtual bool saveEditor(IEditorPart* editor, bool confirm)=0;

	/**
	* Saves the visible views, their layout, and the visible action sets for
	* this page to the current perspective descriptor. The contents of the
	* current perspective descriptor are overwritten.
	*/
	virtual void savePerspective()=0;

	/**
	* Saves the visible views, their layout, and the visible action sets for
	* this page to the given perspective descriptor. The contents of the given
	* perspective descriptor are overwritten and it is made the current one for
	* this page.
	* 
	* @param perspective
	*            the perspective descriptor to save to
	*/
	virtual void savePerspectiveAs(IPerspectiveDescriptor* perspective)=0;

	/**
	* Show or hide the editor area for the page's active perspective.
	* 
	* @param showEditorArea
	*            <code>true</code> to show the editor area,
	*            <code>false</code> to hide the editor area
	*/
	virtual void setEditorAreaVisible(bool showEditorArea)=0;

	/**
	* Changes the visible views, their layout, and the visible action sets
	* within the page to match the given perspective descriptor. This is a
	* rearrangement of components and not a replacement. The contents of the
	* old perspective descriptor are unaffected.
	* <p>
	* When a perspective change occurs the old perspective is deactivated
	* (hidden)=0 and cached for future reference. Then the new perspective is
	* activated (shown)=0. The views within the page are shared by all existing
	* perspectives to make it easy for the user to switch between one
	* perspective and another quickly without loss of context.
	* </p>
	* <p>
	* During activation the action sets are modified. If an action set is
	* specified in the new perspective which is not visible in the old one it
	* will be created. If an old action set is not specified in the new
	* perspective it will be disposed.
	* </p>
	* <p>
	* The visible views and their layout within the page also change. If a view
	* is specified in the new perspective which is not visible in the old one a
	* new instance of the view will be created. If an old view is not specified
	* in the new perspective it will be hidden. This view may reappear if the
	* user selects it from the View menu or if they switch to a perspective
	* (which may be the old one)=0 where the view is visible.
	* </p>
	* <p>
	* The open editors are not modified by this method.
	* </p>
	* 
	* @param perspective
	*            the perspective descriptor
	*/
	virtual void setPerspective(IPerspectiveDescriptor* perspective)=0;

	/**
	* Shows an action set in this page.
	* <p>
	* In most cases where this method is used the caller is tightly coupled to
	* a particular action set. They define it in the registry and may make it
	* visible in certain scenarios by calling <code>showActionSet</code>. A
	* static variable is often used to identify the action set id in caller
	* code.
	* </p>
	* 
	* @param actionSetID
	*            the action set to show
	*/
	virtual void showActionSet(QString actionSetID)=0;

	/**
	* Shows the view identified by the given view id in this page and gives it
	* focus. If there is a view identified by the given view id (and with no
	* secondary id)=0 already open in this page, it is given focus.
	* 
	* @param viewId
	*            the id of the view extension to use
	* @return the shown view
	* @exception PartInitException
	*                if the view could not be initialized
	*/
	virtual IViewPart* showView(QString viewId)=0 ;

	/**
	* Shows a view in this page with the given id and secondary id. The
	* behaviour of this method varies based on the supplied mode. If
	* <code>VIEW_ACTIVATE</code> is supplied, the view is focus. If
	* <code>VIEW_VISIBLE</code> is supplied, then it is made visible but not
	* given focus. Finally, if <code>VIEW_CREATE</code> is supplied the view
	* is created and will only be made visible if it is not created in a folder
	* that already contains visible views.
	* <p>
	* This allows multiple instances of a particular view to be created. They
	* are disambiguated using the secondary id. If a secondary id is given, the
	* view must allow multiple instances by having specified
	* allowMultiple="true" in its extension.
	* </p>
	* 
	* @param viewId
	*            the id of the view extension to use
	* @param secondaryId
	*            the secondary id to use, or <code>null</code> for no
	*            secondary id
	* @param mode
	*            the activation mode. Must be {@link #VIEW_ACTIVATE},
	*            {@link #VIEW_VISIBLE} or {@link #VIEW_CREATE}
	* @return a view
	* @exception PartInitException
	*                if the view could not be initialized
	* @exception IllegalArgumentException
	*                if the supplied mode is not valid
	* @since 3.0
	*/
	virtual IViewPart* showView(QString viewId, QString secondaryId, int mode)=0;

	/**
	* Returns <code>true</code> if the editor is pinned and should not be
	* reused.
	* 
	* @param editor
	*            the editor to test
	* @return bool whether the editor is pinned
	*/
	virtual bool isEditorPinned(IEditorPart* editor)=0;

	/**
	* Returns the number of open editors before reusing editors.
	* 
	* @return a int
	* 
	* @deprecated
	*/
	virtual int getEditorReuseThreshold()=0;

	/**
	* Set the number of open editors before reusing editors. If < 0 the user
	* preference settings will be used.
	* 
	* @param openEditors
	*            the threshold
	* @deprecated use IPageLayout.setEditorReuseThreshold(int openEditors)=0
	*             instead.
	*/
	virtual void setEditorReuseThreshold(int openEditors)=0;

	
	/**
	* Returns an array of IViewParts that are stacked with the given part.
	* 
	* @param part
	*            the part to test
	* @return the parts that are stacked with this part, including the part in
	*         question. <code>null</code> is returned if the part does not
	*         belong to this page.
	* @since 3.0
	*/
	//IViewPart* getViewStack(IViewPart* part)=0;

	/**
	* Returns the new wizard shortcuts associated with the current perspective.
	* Returns an empty array if there is no current perspective.
	* 
	* @see IPageLayout#addNewWizardShortcut(String)=0
	* @return an array of wizard identifiers
	* @since 3.1
	*/
	virtual QString* getNewWizardShortcuts()=0;

	/**
	* Returns the perspective shortcuts associated with the current
	* perspective. Returns an empty array if there is no current perspective.
	* 
	* @see IPageLayout#addPerspectiveShortcut(String)=0
	* @return an array of perspective identifiers
	* @since 3.1
	*/
	virtual QString* getPerspectiveShortcuts()=0;

	/**
	* Returns the show view shortcuts associated with the current perspective.
	* Returns an empty array if there is no current perspective.
	* 
	* @see IPageLayout#addShowViewShortcut(String)=0
	* @return an array of view identifiers
	* @since 3.1
	*/
	virtual QString* getShowViewShortcuts()=0;

	/**
	* Returns the descriptors for the perspectives that are open in this page,
	* in the order in which they were opened.
	* 
	* @return the open perspective descriptors, in order of opening
	* @since 3.1
	*/
	virtual IPerspectiveDescriptor* getOpenPerspectives()=0;

	/**
	* Returns the descriptors for the perspectives that are open in this page,
	* in the order in which they were activated (oldest first)=0.
	* 
	* @return the open perspective descriptors, in order of activation
	* @since 3.1
	*/
	virtual IPerspectiveDescriptor* getSortedPerspectives()=0;

	/**
	* Closes the specified perspective in this page. If the last perspective in
	* this page is closed, then all editors are closed. Views that are not
	* shown in other perspectives are closed as well. If <code>saveParts</code>
	* is <code>true</code>, the user will be prompted to save any unsaved
	* changes for parts that are being closed. The page itself is closed if
	* <code>closePage</code> is <code>true</code>.
	* 
	* @param desc
	*            the descriptor of the perspective to be closed
	* @param saveParts
	*            whether the page's parts should be saved if closed
	* @param closePage
	*            whether the page itself should be closed if last perspective
	* @since 3.1
	*/
	virtual void closePerspective(IPerspectiveDescriptor* desc,
		bool saveParts, bool closePage)=0;

	/**
	* Closes all perspectives in this page. All editors are closed, prompting
	* to save any unsaved changes if <code>saveEditors</code> is
	* <code>true</code>. The page itself is closed if <code>closePage</code>
	* is <code>true</code>.
	* 
	* @param saveEditors
	*            whether the page's editors should be saved
	* @param closePage
	*            whether the page itself should be closed
	* @since 3.1
	*/
	virtual void closeAllPerspectives(bool saveEditors, bool closePage)=0;


	/**
	* Return the visible working sets for this page. Please note that this
	* array is not filtered by activities. Clients should attempt to ensure
	* that any use of this method is consistant with the currently enabled
	* activity set.
	* 
	* @return the visible working sets for this page
	* @see IWorkbench#getActivitySupport()=0
	* @since 3.2
	*/
	virtual IWorkingSet* getWorkingSets()=0;

	/**
	* Set the working sets for this page. Any duplicate entries will be removed
	* from the array by this method.
	* 
	* @param sets
	*            the new working sets for this page. The array may be empty,
	*            but no element in the array may be <code>null</code>.
	* @since 3.2
	*/
	virtual void setWorkingSets(IWorkingSet* sets)=0;

	/**
	* Return a working set that contains all of the elements contained in the
	* array of working sets provided by {@link #getWorkingSets()=0}. Should this
	* array or the underlying elements in any of the working sets change this
	* set will be updated.
	* 
	* <p>
	* This working set is never <code>null</code>, even if there are no
	* working sets assigned to this page via
	* {@link #setWorkingSets(IWorkingSet[])=0}. It is recommended that any
	* client that uses this API be aware of this and act accordingly.
	* Specifically, it is recommended that any client utilizing this or any
	* other IWorkingSet whose {@link IWorkingSet#isAggregateWorkingSet()=0}
	* returns <code>true</code> act as if they are not using any working set
	* if the set is empty. These clients should also maintain an awareness of
	* the contents of aggregate working sets and toggle this behavior should
	* the contents of the aggregate either become empty or non-empty.
	* </p>
	* <p>
	* Example: <br/> Here we have pseudocode showing how some workingset
	* utilizing component could react to changes in aggregate working sets.
	* <br/> <code>
	* private IWorkingSet myWorkingSet;
	* 
	* IPropertyChangeListener workingSetListener = new IPropertyChangeListener()=0 {
	*	virtual void propertyChange(PropertyChangeEvent event)=0 {
	* 		if (isMyCurrentWorkingSet(event)=0)=0 {
	* 			if (isEmptyAggregate(myWorkingSet)=0)=0 {
	* 				showNoSet()=0;
	* 			}
	* 			else {
	* 				showSet()=0;
	* 			}
	*		}
	*	}
	* };
	* 
	* virtual void setWorkingSet(IWorkingSet newSet)=0 {
	* 		myWorkingSet = newSet;
	* 		if (myWorkingSet == null || isEmptyAggregate(myWorkingSet)=0)=0{
	* 			showNoSet()=0;
	* 		}
	* 		else {
	* 			showSet()=0;
	* 		}
	* }
	* </code>
	* </p>
	* 
	* @return the aggregate working set for this page, this implements 
	*   {@link IAggregateWorkingSet}
	* @see IAggregateWorkingSet
	* @since 3.2
	*/
	virtual IWorkingSet* getAggregateWorkingSet()=0;

	/**
	* Returns the page "zoomed" state.
	* 
	* @return <code>true</code> if the page is zoomed in the workbench
	*         window, <code>false</code> otherwise.
	* @since 3.2
	*/
	virtual bool isPageZoomed()=0;

	/**
	* Zooms out the zoomed-in part. If the page does not have a zoomed part, it
	* does nothing.
	* 
	* @since 3.2
	*/
	virtual void zoomOut()=0;

	/**
	* Zoom the page in on a part. If the part is already in zoom then zoom out.
	* 
	* @param ref
	*            the workbench part to zoom in on. Must not be
	*            <code>null</code>.
	* @since 3.2
	*/
	virtual void toggleZoom(IWorkbenchPartReference* ref)=0;

	/**
	* Returns the maximized/minimized/restored state of the given part
	* reference.
	* 
	* @param ref
	*            the workbench part to query. Must not be <code>null</code>.
	* @return one of the STATE_* contants.
	* @since 3.2
	*/
	virtual int getPartState(IWorkbenchPartReference* ref)=0;

	/**
	* Set the state of the given part reference. Setting the state of one part
	* can effect the state of other parts.
	* 
	* @param ref
	*            the workbench part reference. Must not be <code>null</code>.
	* @param state
	*            one of the STATE_* constants.
	* @since 3.2
	*/
	virtual void setPartState(IWorkbenchPartReference* ref, int state)=0;

	/**
	* Find the part reference for the given part. A convenience method to
	* quickly go from part to part reference.
	* 
	* @param part
	*            The part to search for. It can be <code>null</code>.
	* @return The reference for the given part, or <code>null</code> if no
	*         reference can be found.
	* @since 3.2
	*/
	virtual IWorkbenchPartReference* getReference(IWorkbenchPart* part)=0;

	/**
	* Add back an open but non-participating editor
	* 
	* @param ref
	*            the editor to re-add. Must be an editor removed using
	*            #hideEditor(IEditorReference)=0, must not have been closed,
	*            and must not be <code>null</code>.
	* @since 3.5
	* @see #hideEditor(IEditorReference)=0
	*/
	virtual void showEditor(IEditorReference* ref)=0;

	/**
	* Remove an open editor, turn it into a non-participating editor.
	* <p>
	* A non-participating editor will not be returned in the list of open
	* editors ({@link #getEditorReferences()=0})=0 and will not be visible in the
	* editor area. However, it will continue to participate in the save
	* lifecycle and may still be closed by some workbench close events.
	* </p>
	* <p>
	* Behaviour for hiding and showing editors from multiple stacks is not
	* defined (and unsupported)=0 at this time.
	* </p>
	* 
	* @param ref
	*            the editor reference to remove. It must be a current open
	*            editor belonging to this page, and must not be
	*            <code>null</code>.
	* @since 3.5
	* @see #showEditor(IEditorReference)=0
	*/
	virtual void hideEditor(IEditorReference* ref)=0;

	/**
	* Opens editors for the given inputs. Only the editor constructed for the first input 
	* gets activated. 
	* <p>
	* The editor type is determined by mapping <code>editorIDs</code> to an editor
	* extension registered with the workbench.  An editor id is passed rather than
	* an editor object to prevent the accidental creation of more than one editor
	* for the same input. It also guarantees a consistent lifecycle for editors,
	* regardless of whether they are created by the user or restored from saved 
	* data.
	* </p><p>
	* The length of the input array and editor ID arrays must be the same. The editors
	* are opened using pairs of { input[i], editorIDs[i] }.
	* </p>
	* @param inputs the editor inputs
	* @param editorIDs the IDs of the editor extensions to use, in the order of inputs
	* @param matchFlags a bit mask consisting of zero or more of the MATCH_* constants OR-ed together
	* @return references to the editors constructed for the inputs. The editors 
	* corresponding to those reference might not be materialized.
	* @exception MultiPartInitException if at least one editor could not be created or initialized
	* @see #MATCH_NONE
	* @see #MATCH_INPUT
	* @see #MATCH_ID
	* @since 3.5
	*/
	virtual IEditorReference* openEditors( IEditorInput* inputs,  QStringList editorIDs, 
		 int matchFlags)=0;
};
#endif