#ifndef POWERLINE_MESSAGEBOX_H
#define POWERLINE_MESSAGEBOX_H

#include <QDialog>
#include <QMouseEvent>
#include <QTimeLine>
namespace Ui {
class Powerline_MessageBox;
}


#define PL_MB_OK		0x01
#define PL_MB_CANCEL	0x02
#define PL_MB_CHKED		0x04
#define PL_MB_NOCHKED   0x08


class Powerline_MessageBox : public QDialog
{
    Q_OBJECT

public:
    explicit Powerline_MessageBox(QWidget *parent = 0);
    ~Powerline_MessageBox();

private:
    Ui::Powerline_MessageBox *ui;


public:
    void setTitle(const QString &);				//Title set
    void setPic(const QString &);			//Picture path set
    void setContent(const QString &);		//Content set

    void showDlg(const QString &title, const QString &content, int nType = PL_MB_OK);			//无图片的
    void information(const QString &title, const QString &content, int nType = PL_MB_OK);		//有图片的

	void openChkBox(const QString &);
	int retChkboxState() const;

	

protected:
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
	void changeEvent(QEvent *);

	void retranslateUi();


    bool    m_bMove;
    QPoint  m_position;
private:
	void initLanguage();
    void initValue();
    void initSlot();
    void initDlgStyle();

    /////other need the called funtion///////////
    void calculateDlgSize(const QString &);			//According to the content calculation dailog box size
    ///////variable/////
    int m_retValue;
    QWidget *m_Widget;

	bool	m_chkBox;
	int		m_nResult;
private slots:
    void slot_btn_ok();
    void slot_btn_cancel();
	void slot_chkBox(int);


};

#endif // POWERLINE_MESSAGEBOX_H
