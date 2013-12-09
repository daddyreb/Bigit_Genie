#ifndef VIEW
#define VIEW
#include "Part.h"

class View : public Part
{
public:
	/**
	 * Returns the value of the '<em><b>Closeable</b></em>' attribute.
	 * The default value is <code>"true"</code>.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Closeable</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Closeable</em>' attribute.
	 * @see #setCloseable(boolean)
	 * @see org.eclipse.e4.workbench.emf.workbench.WorkbenchPackage#getView_Closeable()
	 * @model default="true"
	 * @generated
	 */
	bool isCloseable() = 0;

	/**
	 * Sets the value of the '{@link org.eclipse.e4.workbench.emf.workbench.View#isCloseable <em>Closeable</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Closeable</em>' attribute.
	 * @see #isCloseable()
	 * @generated
	 */
	void setCloseable(bool value) = 0;
};


#endif
