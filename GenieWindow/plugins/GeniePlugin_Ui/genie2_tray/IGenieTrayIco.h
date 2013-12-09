#ifndef IGENIETRAYICO_H
#define IGENIETRAYICO_H

class IGenieTrayIco{
public:
    enum TRAY_UI_COMMAND
    {
        TUIC_CREATE_TRAY
        ,TUIC_SHOW_ICO
        ,TUIC_HIDE_ICO
        ,TUIC_TRANSLATE_ACTION
        ,TUIC_ACTION_STATE
        ,TUIC_TRAY_MESSAGE
        ,TUIC_SET_QSS
        ,TUIC_SET_TOOLTIP
        ,TUIC_APP_QUIT
        ,TUIC_NUM
    };


    virtual ~IGenieTrayIco(){};
    virtual void uiCommand(int cmd,const QString &para=QString())=0;
};
#endif // IGENIETRAYICO_H
