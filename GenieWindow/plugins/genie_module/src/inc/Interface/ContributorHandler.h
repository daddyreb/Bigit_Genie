#ifndef CONTRIBUTORHANDLER
#define CONTRIBUTORHANDLER
#include <QtCore>

class ContributorHandler
{
public:
	/**
	 * Returns the value of the '<em><b>Plugin</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Plugin</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Plugin</em>' attribute.
	 * @see #setPlugin(String)
	 * @see org.eclipse.e4.workbench.emf.workbench.WorkbenchPackage#getContributedHandler_Plugin()
	 * @model
	 * @generated
	 */
	QString getPlugin() = 0;

	/**
	 * Sets the value of the '{@link org.eclipse.e4.workbench.emf.workbench.ContributedHandler#getPlugin <em>Plugin</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Plugin</em>' attribute.
	 * @see #getPlugin()
	 * @generated
	 */
	void setPlugin(QString value) = 0;

	/**
	 * Returns the value of the '<em><b>Classname</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Classname</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Classname</em>' attribute.
	 * @see #setClassname(String)
	 * @see org.eclipse.e4.workbench.emf.workbench.WorkbenchPackage#getContributedHandler_Classname()
	 * @model
	 * @generated
	 */
	QString getClassname() = 0;

	/**
	 * Sets the value of the '{@link org.eclipse.e4.workbench.emf.workbench.ContributedHandler#getClassname <em>Classname</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Classname</em>' attribute.
	 * @see #getClassname()
	 * @generated
	 */
	void setClassname(QString value) = 0;

	/**
	 * Returns the value of the '<em><b>Handler</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Handler</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Handler</em>' attribute.
	 * @see #setHandler(Object)
	 * @see org.eclipse.e4.workbench.emf.workbench.WorkbenchPackage#getContributedHandler_Handler()
	 * @model dataType="org.eclipse.e4.workbench.emf.workbench.Object" transient="true"
	 * @generated
	 */
	QObject* getHandler() = 0;

	/**
	 * Sets the value of the '{@link org.eclipse.e4.workbench.emf.workbench.ContributedHandler#getHandler <em>Handler</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Handler</em>' attribute.
	 * @see #getHandler()
	 * @generated
	 */
	void setHandler(QObject value) = 0;
};

#endif