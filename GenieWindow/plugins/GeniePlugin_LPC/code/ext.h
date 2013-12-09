#ifndef __ext_h__
#define __ext_h__

#include <map>
#include <QByteArray>
#include <QUiLoader>
class SimpleTranslator
{
public:
	SimpleTranslator();
	~SimpleTranslator();

	bool loadDefaultDict(const QString& path);
	bool loadDict(const QString& path);
	void unloadDict();

	QString translate(const char *text);

private:
	typedef std::map<QByteArray, QString> DictType;
	static bool loadPhraseBook(const QString& path, DictType& dict);

private:
	DictType m_fallbackDict;
	DictType m_dict;
};

class CustomUILoader
	: public QUiLoader
{
public:
	CustomUILoader(QObject *parent = NULL);
	virtual ~CustomUILoader();

	typedef QWidget *(*WidgetFactoryMethod)(const QString& className, QWidget *parent, const QString& name);

	void registerFactory(const QString& className, WidgetFactoryMethod factoryMethod);

protected:
	virtual QWidget *createWidget(const QString& className, QWidget *parent, const QString& name);
	virtual QLayout *createLayout(const QString& className, QObject *parent, const QString& name);
	virtual QActionGroup *createActionGroup(QObject *parent, const QString& name);
	virtual QAction *createAction(QObject *parent, const QString& name);

private:
	typedef std::map<QString, WidgetFactoryMethod> FactoryMap;
	FactoryMap m_factoryMap;
};

#endif // __ext_h__
