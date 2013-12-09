#ifndef REGISTEROBJECT_H
#define REGISTEROBJECT_H

#include "BaseFactory.h"
#include "Factory.h"
#include <QMap>
#include <QList>
#include <QString>
using namespace svp::instantiation;

template<typename BaseT>
class RegisterObject
{
	friend class RegisterObject;

private:
	/**
	* The map which stores factory objects. The factory objects are responsible for
	* creating instances of a certain type.
	*/
	static QMap<QString,BaseFactory<BaseT>* >* instanceMap;

public:
	RegisterObject();
	virtual ~RegisterObject();

	/**
	* Creates an object of type <code>BaseT</code>.
	*
	* @param key
	*			Defines which type of object is created (the name of the class).
	* 
	* @return
	*			The created object of type <code>BaseT</code>.
	*/
	BaseT* createObject( const QString &key );	

	/**
	* Adds a factory object for a certain class name. This method is only called
	* by <code>REGISTER</code> macro.
	*
	* @param key
	*			The name of the class.
	*
	* @param intantiator
	*			The factory object which creates objects of classes whose name is specified by
	*			first parameter.
	*/
	static void addFactory( const QString &key, BaseFactory<BaseT>* intantiator );


	/**
	* Creates only 'local' objects of classes.
	* 
	* @param key
	*			The name of the class.
	*/
	static BaseT* createLocalObject( const QString &key );

};

template<typename BaseT>
RegisterObject<BaseT>::RegisterObject()
{

}

template< typename BaseT>
RegisterObject<BaseT>::~RegisterObject()
{

}

template< typename BaseT>
QMap<QString,BaseFactory<BaseT>* >* RegisterObject<BaseT>::instanceMap;	


template< typename BaseT>
void RegisterObject<BaseT>::addFactory( const QString &key, BaseFactory<BaseT>* intantiator )
{
	if ( instanceMap == 0 )
	{
		//getLogger().log( Logger::DEBUG, "[ObjectCreator#addFactory] Instance map is null, create it." );
		instanceMap = new QMap<QString,BaseFactory<BaseT>* >;
	}
	//getLogger().log( Logger::DEBUG, "[ObjectCreator#addFactory] Called, key: '%1'", key );
	//(*instanceMap)[key] = intantiator;
	instanceMap->insert(key, intantiator);
	//getLogger().log( Logger::DEBUG, "[ObjectCreator#addFactory] Factory for key '%1' added.", key );		
}

template< typename BaseT>
BaseT* RegisterObject<BaseT>::createObject( const QString &key )
{	

	// do only local search
	return createLocalObject( key );
	
}


template< typename BaseT>
BaseT* RegisterObject<BaseT>::createLocalObject( const QString &key )
{	
	BaseFactory<BaseT>* intantiator = (*RegisterObject<BaseT>::instanceMap)[key];
	if ( intantiator == 0 )
	{
		//getLogger().log( Logger::ERROR_, "[ObjectCreator#createLocalObject] No intantiator for class available." );					
		//ObjectCreationException exc( "No intantiator for class available." );
		//throw exc;
	}
	return intantiator->newInstance();
}


#define REGISTER_EXTENSION_CLASS(key,subType) REGISTER_ECC_CLASS(key,IExtension,subType)
#define REGISTER_EXTENSIONPOINT_CLASS(key,subType) REGISTER_ECC_CLASS(key,IExtensionPoint,subType)

#define REGISTER_ECC_CLASS(key,baseType,subType) \
class Register##baseType##with##subType \
{ \
public: \
	Register##baseType##with##subType () \
	{\
		RegisterObject<baseType>::addFactory( key, new Factory<baseType,subType> );\
	} \
} Register##baseType##with##subType##Instance; \


#endif