#ifndef PARTSASH
#define PARTSASH
#include <QtCore>

class PartSash {
	/**
	 * Returns the value of the '<em><b>Weights</b></em>' attribute list.
	 * The list contents are of type {@link java.lang.Integer}.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Weights</em>' attribute list isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Weights</em>' attribute list.
	 * @see org.eclipse.e4.workbench.emf.workbench.WorkbenchPackage#getPartSash_Weights()
	 * @model
	 * @generated
	 */
	QList<int> getWeights();
};

#endif
