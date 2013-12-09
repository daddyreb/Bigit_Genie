#ifndef QGENIEPAGEHOME_H
#define QGENIEPAGEHOME_H




class QGeniePageHome : public QWidget,public Singleton<QGeniePageHome>
{
public:
    QGeniePageHome(QWidget *parent);
    ~QGeniePageHome();
protected:
    virtual void	showEvent ( QShowEvent * event );
protected:
    void changeEvent(QEvent* event);
//    QTimer mTimer;
private:
    Ui::QGeniePageHome mUi;

};

#endif // QGENIEPAGEHOME_H
