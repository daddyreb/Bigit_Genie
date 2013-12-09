#include "ext.h"
#include <QFile>
#include <QWidget>
#include <QXmlStreamReader>

CustomTranslator::CustomTranslator(QObject *parent)
: QTranslator(parent)
{
}

CustomTranslator::~CustomTranslator()
{
}

bool CustomTranslator::loadPhraseBook(const QString& path)
{
	QFile qphFile(path);
	if (!qphFile.open(QIODevice::ReadOnly)) {
		return false;
	}

	QXmlStreamReader xmlReader(&qphFile);

	QString elName;
	QString sourceText;
	QString targetText;

	DictType dict;

	while (!xmlReader.atEnd()) {
		QXmlStreamReader::TokenType token = xmlReader.readNext();
		switch (token) {
		case QXmlStreamReader::StartElement:
			elName = xmlReader.name().toString();
			break;
		case QXmlStreamReader::EndElement:
			elName.clear();
			if (xmlReader.name() == "phrase") {
				// TODO: can't be empty
				if (targetText.isEmpty()) {
					targetText.push_back(QString::fromUtf8("\t"));
				}
				dict.insert(DictType::value_type(sourceText.toUtf8(), targetText));
				sourceText.clear();
				targetText.clear();
			}
			break;
		case QXmlStreamReader::Characters:
			if (elName == QString::fromUtf8("source")) {
				sourceText = xmlReader.text().toString();
			} else if (elName == QString::fromUtf8("target")) {
				targetText = xmlReader.text().toString();
			}
			break;
		default:
			break;
		}
	}

	if (xmlReader.hasError()) {
		return false;
	}
/*
	{
		DictType::const_iterator it = dict.begin();
		DictType::const_iterator ie = dict.end();
		for (; it != ie; ++it) {
			qDebug() << it->first;
		}
	}
*/
	m_dict.swap(dict);

	return true;
}

bool CustomTranslator::isEmpty() const
{
	return m_dict.empty();
}

QString CustomTranslator::translate(const char *context, const char *sourceText, const char *disambiguation) const
{
	DictType::const_iterator it = m_dict.find(sourceText);
	if (it != m_dict.end()) {
		return it->second;
	}
	return QTranslator::translate(context, sourceText, disambiguation);
}

CustomUILoader::CustomUILoader(QObject *parent)
: QUiLoader(parent)
{
}

CustomUILoader::~CustomUILoader()
{
}

void CustomUILoader::registerFactory(const QString& className, WidgetFactoryMethod factoryMethod)
{
	m_factoryMap.insert(FactoryMap::value_type(className, factoryMethod));
}

QWidget *CustomUILoader::createWidget(const QString& className, QWidget *parent, const QString& name)
{
	QWidget *w = NULL;

	FactoryMap::const_iterator it = m_factoryMap.find(className);
	if (it != m_factoryMap.end()) {
		w = (*it->second)(className, parent, name);
	}

	if (!w) {
		w = QUiLoader::createWidget(className, parent, name);
	}

	return w;
}

QLayout *CustomUILoader::createLayout(const QString& className, QObject *parent, const QString& name)
{
	return QUiLoader::createLayout(className, parent, name);
}

QActionGroup *CustomUILoader::createActionGroup(QObject *parent, const QString& name)
{
	return QUiLoader::createActionGroup(parent, name);
}

QAction *CustomUILoader::createAction(QObject *parent, const QString& name)
{
	return QUiLoader::createAction(parent, name);
}
