#ifndef STYLE_HINT_DEFINES__
#define STYLE_HINT_DEFINES__


#define STYLE_PROPERTY_HINT "stylehint"
enum STYLE_HINT_ENUM
{
    SHE_FRAME_BG0=0,
    SHE_FRAME_BG1,//1
    SHE_BUTTON_BIG,//2
    SHE_HEAD_LOGO,//3
    SHE_BUTTON_COLOR,//4
    SHE_FRAME_MAIN
};

#define STYLE_PROPERTY_HINT_SUB "stylehintsub"
enum STYLE_HINT_SUB_ENUM
{
    SHSE_FRAME_SEARCH=0,
    SHSE_FRAME_TITLE,//1
    SHSE_FRAME_AD,//2
    SHSE_FRAME_STACK,//3
    SHSE_FRAME_MAIN,//4
    SHSE_FRAME_MOVIE,
    SHSE_FRAME_PLUGIN,  //6
    SHSE_FRAME_PLUGIN_PROXY //7
};

#define STYLE_PRPOPERTY_HINT_BUT "stylehintbut"
enum STYLE_HINT_BUT_ENUM
{
    SHBE_CLOSE,
    SHBE_MINIMIZE,
    SHBE_MAXIMIZE,
    SHBE_NORMALIZE,
	SHBE_ABOUT //4
};

#define STYLE_PROPERTY_HINT_COMBOBOX "stylehintcob"
enum STYLE_HINT_COMBOBOX_ENUM
{
    SHCE_SEARCH,
    SHCE_LANGUAGE//1
};

#define STYLE_HINT_LEVEL "stylehintlevel"
enum STYLE_HINT_LEVEL_ENUM
{
    SHLE_0,//0
    SHLE_1,
    SHLE_2
};

/*big button bg*/
#define STYLE_HINT_BIGBUT_STATE_BG "stylehintbigbut_state_bg"
enum STYLE_HINT_BBSB{
	SHBBSB_GOOD,
	SHBBSB_WARRING,
	SHBBSB_ERROR
};

/*big button icon*/
#define STYLE_HINT_BIGBUT_STATE_HEAD "stylehintbigbut_state_head"
enum STYLE_HINT_BBSI
{
	SHBBSH_GOOD,
	SHBBSH_WARRING,
	SHBBSH_ERROR
};

/*big button state color*/
#define STYLE_HINT_BIGBUT_STATE_TEXT "stylehintbigbut_state_text"
enum STYLE_HINT_BBST
{
	SHBBST_GOOD,
	SHBBST_WARRING,
	SHBBST_ERROR
};

#endif //STYLE_HINT_DEFINES__