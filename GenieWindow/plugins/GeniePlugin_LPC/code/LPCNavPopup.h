#ifndef __LPCNavPopup_h__
#define __LPCNavPopup_h__

#include <QFrame>

class LPCMainFrame;
class LPCPopupFrame;

class LPCNavPopup
	: public QFrame
{
	Q_OBJECT

public:
	LPCNavPopup(QWidget *parent);
	virtual ~LPCNavPopup();

	void internalInit(LPCMainFrame *mainFrame);
	void internalInit2(LPCPopupFrame *popupFrame);
	void setCaption(const QString& caption);
	QString caption() const;

	virtual void retranslateUI();
	virtual void onPopup() {}

	QString ttt(const char *sourceText, const char *disambiguation = 0, int n = -1);

public slots:
	void pop();

protected:
	virtual void initImpl() { retranslateUI(); }

private:
	LPCMainFrame *m_mainFrame;
	LPCPopupFrame *m_popupFrame;
	QString m_caption;
};

#endif // __LPCNavPopup_h__
