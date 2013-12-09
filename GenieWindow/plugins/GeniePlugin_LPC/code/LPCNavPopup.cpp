#include "LPCMainFrame.h"
#include "LPCNavPopup.h"

LPCNavPopup::LPCNavPopup(QWidget *parent)
: m_mainFrame(NULL), m_popupFrame(NULL)
{
}

LPCNavPopup::~LPCNavPopup()
{
}

void LPCNavPopup::internalInit(LPCMainFrame *mainFrame)
{
	m_mainFrame = mainFrame;
	initImpl();
}

void LPCNavPopup::internalInit2(LPCPopupFrame *popupFrame)
{
	m_popupFrame = popupFrame;
}

void LPCNavPopup::setCaption(const QString& caption)
{
	if (m_caption != caption) {
		m_caption = caption;
		if (m_popupFrame) {
			m_popupFrame->changeCaption(caption);
		}
	}
}

QString LPCNavPopup::caption() const
{
	return m_caption;
}

void LPCNavPopup::retranslateUI()
{
}

void LPCNavPopup::pop()
{
	if (m_mainFrame) {
		m_mainFrame->pop(this);
	}
}

QString LPCNavPopup::ttt(const char *sourceText, const char *disambiguation, int n)
{
	if (!m_mainFrame) {
        return QString::fromLatin1(sourceText);
	}
	return m_mainFrame->ttt(sourceText, disambiguation, n);
}
