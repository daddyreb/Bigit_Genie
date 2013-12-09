#ifndef __ext_h__
#define __ext_h__

#include <map>
#include <QByteArray>
#include <QTranslator>
#include <QUiLoader>

class CustomTranslator
	: public QTranslator
{
public:
	CustomTranslator(QObject *parent = NULL);
	virtual ~CustomTranslator();

	bool loadPhraseBook(const QString& path);

	virtual bool isEmpty() const;
	virtual QString translate(const char *context, const char *sourceText, const char *disambiguation) const;

private:
	typedef std::map<QByteArray, QString> DictType;
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
