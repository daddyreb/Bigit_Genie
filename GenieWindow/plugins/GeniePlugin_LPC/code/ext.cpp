#include "ext.h"
#include <QFile>
#include <QWidget>
#include <QXmlStreamReader>
#include <QtCore/QLocale>

#ifdef EXPORT_QPH_PLEASE

#include <algorithm>
#include <list>
#include <QTextStream>
#include <QFile>

struct Row
{
	QString n;
	QString d[20];
};

struct ColumnMatch
{
	ColumnMatch(const QByteArray& c)
		: m_c(QString::fromUtf8(c.data()))
	{
	}

	bool operator()(const Row& r)
	{
		return r.n == m_c;
	}

	QString m_c;
};

SimpleTranslator::SimpleTranslator()
{
	struct LANG_BIND
	{
		const char *code;
		const char *name;
		int langId;
	};

	static const LANG_BIND g_languages[] = {
		{ "csy", "cs", QLocale::Czech },
		{ "dan", "da", QLocale::Danish },
		{ "deu", "de", QLocale::German },
		{ "ell", "el", QLocale::Greek },
		{ "enu", "en", QLocale::English },
		{ "esp", "es", QLocale::Spanish },
		{ "fin", "fi", QLocale::Finnish },
		{ "hun", "hu", QLocale::Hungarian },
		{ "ita", "it", QLocale::Italian },
		{ "nld", "nl", QLocale::Dutch },
		{ "nor", "no", QLocale::Norwegian },
		{ "plk", "pl", QLocale::Polish },
		{ "ptb", "pt_BR", QLocale::Portuguese },
		{ "rom", "ro", QLocale::Romanian },
		{ "rus", "ru", QLocale::Russian },
		{ "sky", "sk", QLocale::Slovak },
		{ "slv", "sl", QLocale::Slovenian },
		{ "sve", "sv", QLocale::Swedish },
	};

	std::list<Row> ls;

	for (size_t i = 0; i < sizeof(g_languages) / sizeof(g_languages[0]); i++) {
		DictType dict;
		if (loadPhraseBook(QString::fromUtf8(":/LPCModule/i18n/%1.qph").arg(g_languages[i].name), dict)) {
			DictType::const_iterator it = dict.begin();
			DictType::const_iterator ie = dict.end();
			for (; it != ie; ++it) {
				std::list<Row>::iterator itL = std::find_if(ls.begin(), ls.end(), ColumnMatch(it->first));
				if (itL != ls.end()) {
					itL->d[i] = it->second;
				} else {
					Row r;
					r.n = QString::fromUtf8(it->first.data());
					r.d[i] = it->second;
					ls.push_back(r);
				}
			}
		}
	}

	QString output("<html>\n<head></head>\n<body>\n<table>\n<tr><td>V</td>");
	for (int i = 0; i < 18; i++) {
		output.append("<td>");
		output.append(g_languages[i].code);
		output.append("</td>");
	}
	output.append("</tr>\n");

	std::list<Row>::const_iterator ita = ls.begin();
	std::list<Row>::const_iterator iea = ls.end();
	for (; ita != iea; ++ita) {
		output.append("<tr>\n<td>");
		output.append(ita->n);
		output.append("</td>");
		for (int i = 0; i < 18; i++) {
			output.append("<td>");
			output.append(ita->d[i]);
			output.append("</td>");
		}
		output.append("</tr>\n");
	}

	output.append("</table>\n</body>\n</html>");

	QFile fo("d:/qph_export.html");
	fo.open(QIODevice::WriteOnly);
	QTextStream ts(&fo);
	ts.setCodec("utf-8");
	ts << output;

}

#else

SimpleTranslator::SimpleTranslator()
{
}

#endif // EXPORT_QPH_PLEASE

SimpleTranslator::~SimpleTranslator()
{
}

bool SimpleTranslator::loadDefaultDict(const QString& path)
{
	DictType dict;
	if (!loadPhraseBook(path, dict)) {
		return false;
	}

	m_fallbackDict.swap(dict);
	return true;
}

bool SimpleTranslator::loadDict(const QString& path)
{
	DictType dict;
	if (!loadPhraseBook(path, dict)) {
		return false;
	}

	m_dict.swap(dict);
	return true;
}

void SimpleTranslator::unloadDict()
{
	m_dict.clear();
}

QString SimpleTranslator::translate(const char *text)
{
	QString result;
	DictType::const_iterator it = m_dict.find(text);
	if (it != m_dict.end()) {
		result = it->second;
	} else {
		it = m_fallbackDict.find(text);
		if (it != m_fallbackDict.end()) {
			result = it->second;
		} else {
			result = QString::fromUtf8(text);
		}
	}

	return result;
}

bool SimpleTranslator::loadPhraseBook(const QString& path, DictType& dict)
{
	QFile qphFile(path);
	if (!qphFile.open(QIODevice::ReadOnly)) {
		return false;
	}

	QXmlStreamReader xmlReader(&qphFile);

	QString elName;
	QString sourceText;
	QString targetText;

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

	return true;
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
