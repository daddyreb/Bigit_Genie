#include "LPCMainFrame.h"
#include "LPCNavPage.h"
#include <QtCore/QVariant>
#include <QAbstractButton>
#include <QLabel>

QString markupLink(const QString& text)
{
	QString result;
	//result.replace(QRegExp(QString::fromUtf8("(http://[^ ]+)([ .,!?])")), QString::fromUtf8("<a href=\"\\1\">\\1</a>\\2"));
	static const QString sep = QString::fromLatin1(",!?");
	int offset = 0;
	for (;;) {
		int pos1 = text.indexOf(QLatin1String("http://"), offset);
		if (pos1 < 0) {
			result.append(text.mid(offset));
			//qDebug() << result;
			break;
		} else {
			result.append(text.mid(offset, pos1 - offset));
			//qDebug() << result;
			offset = pos1 + 7;

			bool domain = false;

			while (offset < text.length()) {
				QChar c = text[offset];
				if (c.isSpace() || c.unicode() >= 256 || sep.indexOf(c) >= 0) {
					break;
				}

				if (!domain && c == '/') {
					domain = true;
				}

				if (domain && c == '.') {
					break;
				}

				offset++;
			}

			QString href = text.mid(pos1, offset - pos1);

			result.append(QLatin1String("<a href=\""));
			result.append(href);
			result.append(QLatin1String("\">"));
			result.append(href);
			result.append(QLatin1String("</a>"));

			//qDebug() << text.mid(pos1, offset - pos1);
		}
	}
	return result;
}

LPCNavPage::LPCNavPage(QWidget *parent)
: QFrame(parent), m_mainFrame(NULL)
{
}

LPCNavPage::~LPCNavPage()
{
}

void LPCNavPage::init(LPCMainFrame *mainFrame)
{
	m_mainFrame = mainFrame;
	retranslateUI();
}

bool LPCNavPage::prepare()
{
    QPushButton *abortBtn = this->findChild<QPushButton*>(QString::fromUtf8("abort"));
	if (abortBtn) {
		abortBtn->setVisible(appData()->needShowAbortButton());
	}
        return prepareImpl();
}

AppData *LPCNavPage::appData() const
{
	return m_mainFrame->appData();
}

CoreApi *LPCNavPage::coreApi() const
{
	return m_mainFrame->coreApi();
}

WebApi *LPCNavPage::webApi() const
{
	return m_mainFrame->coreApi()->webApi();
}

RouterApi *LPCNavPage::routerApi() const
{
	return m_mainFrame->coreApi()->routerApi();
}

void LPCNavPage::updateTextUI(QWidget *widget)
{
	updateTextUIWork(widget);

	QObjectList::const_iterator it = widget->children().begin();
	QObjectList::const_iterator ie = widget->children().end();
	for (; it != ie; ++it) {
		QWidget *childWidget = qobject_cast<QWidget*>(*it);
		if (childWidget) {
			updateTextUI(childWidget);
		}
	}
}

void LPCNavPage::updateWidgetText(QWidget *widget, const QString& text)
{
	if (QLabel *lbl = qobject_cast<QLabel*>(widget)) {
		lbl->setText(text);
	} else if (QAbstractButton *btn = qobject_cast<QAbstractButton*>(widget)) {
		btn->setText(text);
	}
}

void LPCNavPage::retranslateUI()
{
	updateTextUI(this);
}

QString LPCNavPage::ttt(const char *sourceText, const char *disambiguation, int n)
{
	if (!m_mainFrame) {
        return QString::fromLatin1(sourceText);
	}
	return m_mainFrame->ttt(sourceText, disambiguation, n);
}

void LPCNavPage::navigateTo(const char *pageName, bool force)
{
	app()->requestNavigateTo(QString::fromUtf8(pageName), force);
}

void LPCNavPage::setLPCText(QWidget *widget, const char *text, bool markup)
{
	widget->setProperty("LPCText", QString::fromUtf8(text));
	widget->setProperty("LPCTextMarkup", markup);
	updateTextUIWork(widget);
}

void LPCNavPage::reportStatus(int status)
{
	app()->reportStatus(status);
}

void LPCNavPage::updateTextUIWork(QWidget *widget)
{
	QVariant vLPCText = widget->property("LPCText");
	QVariant vLPCTextMarkup = widget->property("LPCTextMarkup");

	if (vLPCText.isValid()) {
		if (vLPCTextMarkup.isValid() && vLPCTextMarkup.toBool()) {
			updateWidgetText(widget, markupLink(ttt(vLPCText.toByteArray().data())));
		} else {
			updateWidgetText(widget, ttt(vLPCText.toByteArray().data()));
		}
	}
}

void LPCNavPage::gotoMainPage()
{
	navigateTo("manage");
}
