#ifndef EFACTORY
#define EFACTORY
#include <QtCore>

class EPackage;
class EClass;
class EDataType;

class EFactory {
  /**
   * Returns the value of the '<em><b>EPackage</b></em>' reference.
   * It is bidirectional and its opposite is '{@link org.eclipse.emf.ecore.EPackage#getEFactoryInstance <em>EFactory Instance</em>}'.
   * <!-- begin-user-doc -->
   * <p>
   * It represent the package of this factory.
   * </p>
   * <!-- end-user-doc -->
   * @return the value of the '<em>EPackage</em>' reference.
   * @see #setEPackage(EPackage)
   * @see org.eclipse.emf.ecore.EcorePackage#getEFactory_EPackage()
   * @see org.eclipse.emf.ecore.EPackage#getEFactoryInstance
   * @model opposite="eFactoryInstance" resolveProxies="false" required="true" transient="true"
   * @generated
   */
  virtual EPackage* getEPackage() = 0;

  /**
   * Sets the value of the '{@link org.eclipse.emf.ecore.EFactory#getEPackage <em>EPackage</em>}' reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @param value the new value of the '<em>EPackage</em>' reference.
   * @see #getEPackage()
   * @generated
   */
  virtual void setEPackage(EPackage* value) = 0;

  /**
   * <!-- begin-user-doc -->
   * Creates a new instance of the class and returns it.
   * @param eClass the class of the new instance.
   * @return a new instance of the class.
   * <!-- end-user-doc -->
   * @model
   * @generated
   */
  virtual QObject* create(EClass* eClass) = 0;

  /**
   * <!-- begin-user-doc -->
   * Creates an instance of the data type from the literal value.
   * @param literalValue literal value of the instance.
   * @return an instance of the data type.
   * @see #convertToString(EDataType, Object)
   * @ignore
   * <!-- end-user-doc -->
   * @model
   * @generated
   */
  virtual QObject* createFromString(EDataType* eDataType, QString literalValue) = 0;

  /**
   * <!-- begin-user-doc -->
   * Returns the literal value of the instance.
   * @param eDataType the data type of the value.
   * @param instanceValue the value.
   * @return the literal value of the instance.
   * @see #createFromString(EDataType, String)
   * @ignore
   * <!-- end-user-doc -->
   * @model
   * @generated
   */
  virtual QString convertToString(EDataType* eDataType, QObject* instanceValue) = 0;

};

#endif
