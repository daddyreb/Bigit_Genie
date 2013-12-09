#ifndef WORKBENCHMODEL
#define WORKBENCHMODEL
#include <QtCore>

class WorkbenchWindow;

class WorkbenchModel {
	/**
	 * Returns the value of the '<em><b>Windows</b></em>' containment reference list.
	 * The list contents are of type {@link org.eclipse.e4.workbench.emf.workbench.WorkbenchWindow}.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Windows</em>' containment reference list isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Windows</em>' containment reference list.
	 * @see org.eclipse.e4.workbench.emf.workbench.WorkbenchPackage#getWorkbenchModel_Windows()
	 * @model containment="true"
	 * @generated
	 */
	QList<WorkbenchWindow*> getWindows() = 0;

	/**
	 * Returns the value of the '<em><b>Active WBW</b></em>' reference.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Active WBW</em>' reference isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Active WBW</em>' reference.
	 * @see #setActiveWBW(WorkbenchWindow)
	 * @see org.eclipse.e4.workbench.emf.workbench.WorkbenchPackage#getWorkbenchModel_ActiveWBW()
	 * @model
	 * @generated
	 */
	WorkbenchWindow* getActiveWBW() = 0;

	/**
	 * Sets the value of the '{@link org.eclipse.e4.workbench.emf.workbench.WorkbenchModel#getActiveWBW <em>Active WBW</em>}' reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Active WBW</em>' reference.
	 * @see #getActiveWBW()
	 * @generated
	 */
	void setActiveWBW(WorkbenchWindow* value) = 0;
};

#endif