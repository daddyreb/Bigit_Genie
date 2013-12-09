#ifndef __QSTYLEMANAGER__
#define __QSTYLEMANAGER__

enum E_STYLE_TYPE{
    EST_DEFAULT=0,
    EST_TEST,
	EST_NUM
};

class QGenieStyleManager:public Singleton<QGenieStyleManager>
{
public:
    QGenieStyleManager();
    ~QGenieStyleManager();
    void setStyle(E_STYLE_TYPE type);
};
#endif
