#ifndef EDATATYPE
#define EDATATYPE

class EDataType {
  /**
   * Returns the value of the '<em><b>Serializable</b></em>' attribute.
   * The default value is <code>"true"</code>.
   * <!-- begin-user-doc -->
   * <p>
   * It represents whether values of this type will be {@link org.eclipse.emf.ecore.resource.Resource#save(java.util.Map) serialized}.
   * For a serializable data type,
   * there will be factory methods of the form:
   *<pre>
   *  String convertXyzToString(EDataType metaObject, Object instanceValue)
   *  Object createXyzFromString(EDataType metaObject, String initialValue)
   *</pre>
   * in the generated factory implementation.
   * Clients will typically need to hand write the bodies of these generated methods.
   * </p>
   * <!-- end-user-doc -->
   * @return the value of the '<em>Serializable</em>' attribute.
   * @see #setSerializable(boolean)
   * @see org.eclipse.emf.ecore.EcorePackage#getEDataType_Serializable()
   * @model default="true"
   * @generated
   */
  bool isSerializable() = 0;

  /**
   * Sets the value of the '{@link org.eclipse.emf.ecore.EDataType#isSerializable <em>Serializable</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @param value the new value of the '<em>Serializable</em>' attribute.
   * @see #isSerializable()
   * @generated
   */
  void setSerializable(bool value) = 0;
};

#endif
