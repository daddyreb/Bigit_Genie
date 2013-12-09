#ifndef __PLUGIN_UI_DEF__
#define __PLUGIN_UI_DEF__

#define STYLE_HINT "stylehintplugin"
#include <QVariant>

#define TEST_BUT
#define TEST_BUT_MAX_W 70
#define TEST_BUT_MAX_H 30
#define TEST_BUT_MIN_W 70
#define TEST_BUT_MIN_H 30
#define TEST_BUT_HINT 10

#define MAINMENU_BIGBUTTON
#define MAINMENU_BIGBUTTON_MAX_W 197
#define MAINMENU_BIGBUTTON_MAX_H 34
#define MAINMENU_BIGBUTTON_MIN_W 197
#define MAINMENU_BIGBUTTON_MIN_H 34
#define MAINMENU_BIGBUTTON_HINT 4

#define TAB_LEFT
#define	TAB_LEFT_MAX_W 199
#define TAB_LEFT_MAX_H 37
#define	TAB_LEFT_MIN_W 199
#define TAB_LEFT_MIN_H 37
#define TAB_LEFT_HINT 5

#define TAB_MID
#define TAB_MID_MAX_W 212
#define TAB_MID_MAX_H 37
#define TAB_MID_MIN_W 212
#define TAB_MID_MIN_H 37
#define TAB_MID_HINT 6

#define TAB_RIGHT
#define TAB_RIGHT_MAX_W 201
#define TAB_RIGHT_MAX_H 37
#define TAB_RIGHT_MIN_W 201
#define TAB_RIGHT_MIN_H 37
#define TAB_RIGHT_HINT	7

#define LITTLE_CLICKER
#define LITTLE_CLICKER_MAX_W 17
#define LITTLE_CLICKER_MAX_H 17
#define LITTLE_CLICKER_MIN_W 17
#define LITTLE_CLICKER_MIN_H 17
#define LITTLE_CLICKER_HINT	8

#define NORMAL_BUTTON
#define NORMAL_BUTTON_MAX_W 19999
#define NORMAL_BUTTON_MAX_H 23
#define NORMAL_BUTTON_MIN_W 40
#define NORMAL_BUTTON_MIN_H 23
#define NORMAL_BUTTON_HINT 9

#define LONG_BUTTON
#define LONG_BUTTON_MAX_W 40
#define LONG_BUTTON_MAX_H 20
#define LONG_BUTTON_MIN_W 40
#define LONG_BUTTON_MIN_H 20
#define LONG_BUTTON_HINT 10

#define GRADIENT_BUTTON
#define GRADIENT_BUTTON_MAX_W 191
#define GRADIENT_BUTTON_MAX_H 34
#define GRADIENT_BUTTON_MIN_W 191
#define GRADIENT_BUTTON_MIN_H 34
#define GRADIENT_BUTTON_HINT 11


#define LITTLE_BUTTON
#define LITTLE_BUTTON_MAX_W 17
#define LITTLE_BUTTON_MAX_H 17
#define LITTLE_BUTTON_MIN_W 17
#define LITTLE_BUTTON_MIN_H 17
#define LITTLE_BUTTON_HINT 12


#define GO_BUTTON
#define GO_BUTTON_MAX_W 36
#define GO_BUTTON_MAX_H 23
#define GO_BUTTON_MIN_W 36
#define GO_BUTTON_MIN_H 23
#define GO_BUTTON_HINT 13

#define MAIN_LABEL
#define	MAIN_LABEL_MAX_W 99999
#define MAIN_LABEL_MAX_H 99999
#define	MAIN_LABEL_MIN_W 0
#define MAIN_LABEL_MIN_H 0
#define MAIN_LABEL_HINT 14

#define TRAY_MENU
#define	TRAY_MENU_MAX_W 99999
#define TRAY_MENU_MAX_H 99999
#define	TRAY_MENU_MIN_W 0
#define TRAY_MENU_MIN_H 0
#define TRAY_MENU_HINT 15

#define PARENT_LABEL1
#define PARENT_LABEL1_HINT 16

#define PARENT_LABEL2
#define PARENT_LABEL2_HINT 17

#define PARENT_LABEL3
#define PARENT_LABEL3_HINT 18

#define FLEX_BUTTON
#define FLEX_BUTTON_HINT 19

#define SET_FIX_SIZE(FLAG,what) \
do{\
   (what)->setMinimumSize(FLAG##_MIN_W,FLAG##_MIN_H);\
   (what)->setMaximumSize(FLAG##_MAX_W,FLAG##_MAX_H);\
   (what)->setProperty(STYLE_HINT, QVariant( FLAG##_HINT));\
}while(0)

#define SET_STYLE_PROPERTY(FLAG,what) \
do{\
   (what)->setProperty(STYLE_HINT, QVariant( FLAG##_HINT));\
}while(0)

#ifdef _WIN32
#define FONT_FLAG "  font-family:'Arial'; font-size:10pt; font-weight:600; font-style:normal;"
#else
#define FONT_FLAG "  font-family:'Arial'; font-size:12pt; font-weight:600; font-style:normal;"
#endif

#define FONT_FLAG_WITHUNDERLINE "  font-family:'Arial'; font-size:9pt; font-weight:600; font-style:normal; text-decoration: underline; "
#define BBT_MODE_CLICKHERE "<span style=\" "FONT_FLAG" color:#40a900;\">%1</span>"
#define BBT_MODE_GOOD "<span style=\" "FONT_FLAG"color:#303030;\">%1</span>"\
    " <span style=\" "FONT_FLAG" color:#40a900;\">%2</span>"
#define BBT_MODE_OVERFLOW "<span style=\"  "FONT_FLAG" color:#303030;\">%1</span>"\
    " <span style=\" "FONT_FLAG"color:blue;\">%2</span>"
#define BBT_MODE_ERROR "<span style=\"  "FONT_FLAG" color:#303030;\">%1</span>"\
    " <span style=\" "FONT_FLAG"color:red;\">%2</span>"
#define BBT_MODE_WANTFIX "<span style=\"  "FONT_FLAG" color:#ffffff;\">%1</span>"\
    " <span style=\" "FONT_FLAG_WITHUNDERLINE"color:#ffffff;\">%2</span>"



#define BK_CSS ".QGenieButtonBig{border-image:url(:/but_state_good.png);}"\
    ".QGenieButtonBig:hover{border-image:url(:/but_state_hover.png);}"\
    "QLabel#label_text{	font:75 17px \"Arial\";	font-weight:bold;}"

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
    "QFrame#fr_head{background-image:url(:/but_head_error.png);}"


#endif
