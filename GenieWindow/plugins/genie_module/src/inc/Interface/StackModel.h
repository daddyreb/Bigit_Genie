#ifndef STACKMODEL
#define STACKMODEL

class Part;

class StackModel 
{
public:
	/**
	 * Returns the value of the '<em><b>Selected Part</b></em>' reference.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Selected Part</em>' reference isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Selected Part</em>' reference.
	 * @see #setSelectedPart(Part)
	 * @see org.eclipse.e4.workbench.emf.workbench.WorkbenchPackage#getStackModel_SelectedPart()
	 * @model
	 * @generated
	 */
	Part* getSelectedPart() = 0;

	/**
	 * Sets the value of the '{@link org.eclipse.e4.workbench.emf.workbench.StackModel#getSelectedPart <em>Selected Part</em>}' reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Selected Part</em>' reference.
	 * @see #getSelectedPart()
	 * @generated
	 */
	void setSelectedPart(Part* value) = 0;
};

#endif
