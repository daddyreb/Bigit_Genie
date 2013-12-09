#include "QGenieButtonBig.h"
#include "QGenieUiObject.h"
#include "genie2_interface.h"
#include "language_Ui.h"

#ifdef _WIN32
#define FONT_FLAG "   font-size:10pt; font-weight:600; font-style:normal;"
#else
#define FONT_FLAG "   font-size:12pt; font-weight:600; font-style:normal;"
#endif

#define FONT_FLAG_WITHUNDERLINE "   font-size:9pt; font-weight:600; font-style:normal; text-decoration: underline; "
#define BBT_MODE_CLICKHERE "<span style=\" "FONT_FLAG" color:#40a900;\">%1 %2</span>"
#define BBT_MODE_GOOD "<span style=\" "FONT_FLAG"color:#303030;\">%1</span>"\
    " <span style=\" "FONT_FLAG" color:#40a900;\">%2</span>"
#define BBT_MODE_OVERFLOW "<span style=\"  "FONT_FLAG" color:#303030;\">%1</span>"\
    " <span style=\" "FONT_FLAG"color:blue;\">%2</span>"
#define BBT_MODE_ERROR "<span style=\"  "FONT_FLAG" color:#303030;\">%1</span>"\
    " <span style=\" "FONT_FLAG"color:red;\">%2</span>"
#define BBT_MODE_WANTFIX "<span style=\"  "FONT_FLAG" color:#ffffff;\">%1</span>"\
    " <span style=\" "FONT_FLAG_WITHUNDERLINE"color:#ffffff;\">%2</span>"



#define BK_CSS ".QGenieButtonBig{border-image:url(:/images/but_state_good.png);}"\
    ".QGenieButtonBig:hover{border-image:url(:/images/but_state_hover.png);}"\
    "QLabel#label_text{	font-size:17px ;font-weight:bold;}"

#define BBT_CSS_GOOD BK_CSS
#define BBT_CSS_ERROR BK_CSS"QFrame#frame_butline{"\
    "background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgb( 243, 42, 50), stop:1 rgb( 206, 37, 42));"\
    "border-style:solid;"\
    "border-width:0px;"\
    "border-bottom-right-radius: 4px;"\
    "border-bottom-left-radius: 4px;"\
    "border-color:rgb(246,233,172);"\
    "margin-left:6px;"\
    "margin-right:6px;"\
    "}"\
    "QFrame#fr_head{background-image:url(:/images/but_head_error.png);}"

#define ICO_QSS  "QFrame#frame_ico{background-image:url(%1);background-repeat: none; background-position: top center;}"

//    enum _BBT_MODES{
//        _BM_CLICKHERE
//        ,_BM_GOOD
//        ,_BM_ERROR
//        ,_BM_WANTFIX
//        ,_BM_OVERFLOW
//        ,_BM_NUM
//        ,_BTQSS_ICO
//        ,_BTOSS_GOOD
//        ,_BTOSS_ERROR

//    };

const char *sModes[]={
  BBT_MODE_CLICKHERE
  ,BBT_MODE_GOOD
  ,BBT_MODE_ERROR
  ,BBT_MODE_WANTFIX
  ,BBT_MODE_OVERFLOW
  ,""

};


QGenieButtonBig::QGenieButtonBig(QWidget *parent)
    :QPushButton(parent)
{
    mUi.setupUi(this);
	setWindowFlags(Qt::FramelessWindowHint);
//    setFocusPolicy(Qt::NoFocus);
    setFlat(true);
	setAttribute(Qt::WA_TranslucentBackground, true);
//	setStyleSheet("QPushButton{border: 3px solid white;border-radius:8px}");
	//mUi.frame_butline->setAttribute(Qt::WA_TranslucentBackground, true);
	//mUi.fr_head->setAttribute(Qt::WA_TranslucentBackground, true);
	//mUi.frame_ico->setAttribute(Qt::WA_TranslucentBackground, true);
	//mUi.frame_text->setAttribute(Qt::WA_TranslucentBackground, true);
	//mUi.label_text->setAttribute(Qt::WA_TranslucentBackground, true);
	//mUi.userText->setAttribute(Qt::WA_TranslucentBackground, true);
}

void QGenieButtonBig::setText(const QString &text)
{
//    mUi.label_text->setText(text);
}

void QGenieButtonBig::updateUi(quint32 flag)
{

    //_tLinkData *t=(_tLinkData *)userData(0);
    //if(t)
    //{
    //    if(flag & UF_BUTLINE)
    //    {
    //        int modeidx=(t->uiPara[_tLinkData::UP_BUTLINE_MODE].type()==QVariant::Int)?
    //                    t->uiPara[_tLinkData::UP_BUTLINE_MODE].toInt():-1;
    //        modeidx=(0<=modeidx && modeidx<sizeof(sModes)/sizeof(sModes[0]))?modeidx:0;
    //        mUi.userText->setText(QString(sModes[modeidx]).arg(GENIE2_VARIANT_TO_STRING(t->uiPara[_tLinkData::UP_BUTLINE_PARA1],t->uuid_idx)
    //                                                  ,GENIE2_VARIANT_TO_STRING(t->uiPara[_tLinkData::UP_BUTLINE_PARA2],t->uuid_idx)));
    //        this->setStyleSheet((3==modeidx)?BBT_CSS_ERROR:BBT_CSS_GOOD);
    //    }
    //    if(flag & UF_TEXT)
    //    {
    //        mUi.label_text->setText(GENIE2_VARIANT_TO_STRING(t->uiPara[_tLinkData::UP_BBT_LANGUAGE],t->uuid_idx));
    //    }
    //    if(flag & UF_ICO)
    //    {
    //        const QString stylesheet=ICO_QSS;
    //        QString ico=GENIE2_RES("images/bbticon/%1.png").arg(t->uiPara[_tLinkData::UP_ICO].toString());

    //        if(!QFile::exists(ico))
    //        {
    //            ico=GENIE2_RES("images/bbticon/bbt_default.png");
    //        }


    //       mUi.frame_ico->setStyleSheet(stylesheet.arg(ico));
			 ////mUi.frame_ico->setStyleSheet(stylesheet.arg("X:/Genie1.2/trunk/testplugin/images/bbticon/Map.png"));
    //    }
    //}
}

