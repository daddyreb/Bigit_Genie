#ifndef ITEMMODEL
#define ITEMMODEL

class ItemModel 
{
	/**
	 * Returns the value of the '<em><b>Separator</b></em>' attribute.
	 * The default value is <code>"false"</code>.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Separator</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Separator</em>' attribute.
	 * @see #setSeparator(boolean)
	 * @see org.eclipse.e4.workbench.emf.workbench.WorkbenchPackage#getItemModel_Separator()
	 * @model default="false"
	 * @generated
	 */
	bool isSeparator() = 0;

	/**
	 * Sets the value of the '{@link org.eclipse.e4.workbench.emf.workbench.ItemModel#isSeparator <em>Separator</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Separator</em>' attribute.
	 * @see #isSeparator()
	 * @generated
	 */
	void setSeparator(bool value) = 0;

	/**
	 * Returns the value of the '<em><b>Handler</b></em>' reference.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Handler</em>' reference isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Handler</em>' reference.
	 * @see #setHandler(ContributedHandler)
	 * @see org.eclipse.e4.workbench.emf.workbench.WorkbenchPackage#getItemModel_Handler()
	 * @model
	 * @generated
	 */
	//ContributedHandler getHandler();

	/**
	 * Sets the value of the '{@link org.eclipse.e4.workbench.emf.workbench.ItemModel#getHandler <em>Handler</em>}' reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Handler</em>' reference.
	 * @see #getHandler()
	 * @generated
	 */
	//void setHandler(ContributedHandler value);
};


#endif