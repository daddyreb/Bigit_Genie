#include "logger.h"
#include "plugin.h"
#include <QtPlugin>
#include <QMessageBox>

#include "version.h"

enum {
	LPC_I18N_PLUGIN_NAME=1300,
	LPC_I18N_STATUS_LABEL,
	LPC_I18N_STATUS_NAME_DISABLED,
	LPC_I18N_STATUS_NAME_ENABLED,
	LPC_I18N_CLICK,
	LPC_I18N_ABORT,
	LPC_I18N_BASIC_BACK,
	LPC_I18N_CANCEL,
	LPC_I18N_CREATEACCOUNT_CHECK,
	LPC_I18N_CREATEACCOUNT_CHECK_AVAILABLE,
	LPC_I18N_CREATEACCOUNT_CHECK_UNAVAILABLE,
	LPC_I18N_CREATEACCOUNT_EMAIL,
	LPC_I18N_CREATEACCOUNT_EMAIL_CONFIRM,
	LPC_I18N_CREATEACCOUNT_EMAIL_INVALID,
	LPC_I18N_CREATEACCOUNT_EMAIL_TAKEN,
	LPC_I18N_CREATEACCOUNT_EMAIL_TAKEN_SIGNIN,
	LPC_I18N_CREATEACCOUNT_ERROR,
	LPC_I18N_CREATEACCOUNT_ERROR_DEVICE,
	LPC_I18N_CREATEACCOUNT_ERROR_PASSWORD,
	LPC_I18N_CREATEACCOUNT_ERROR_USERNAME,
	LPC_I18N_CREATEACCOUNT_PASSWORD,
	LPC_I18N_CREATEACCOUNT_PASSWORD_CONFIRM,
	LPC_I18N_CREATEACCOUNT_STATUS,
	LPC_I18N_CREATEACCOUNT_SUBTITLE,
	LPC_I18N_CREATEACCOUNT_TITLE,
	LPC_I18N_CREATEACCOUNT_USERNAME,
	LPC_I18N_CUSTOM_START,
	LPC_I18N_CUSTOM_SUBTITLE,
	LPC_I18N_CUSTOM_TEXT_1,
	LPC_I18N_CUSTOM_TEXT_2,
	LPC_I18N_CUSTOM_TITLE,
	LPC_I18N_DEVICE_API_CREATE,
	LPC_I18N_DEVICE_API_GET,
	LPC_I18N_DEVICE_ERROR_DEVICE_ID_NOT_MINE,
	LPC_I18N_DEVICE_ERROR_DEVICE_KEY,
	LPC_I18N_DEVICE_ERROR_NETGEAR,
	LPC_I18N_DEVICE_ROUTER_GET,
	LPC_I18N_DEVICE_ROUTER_INIT,
	LPC_I18N_DEVICE_ROUTER_SET,
	LPC_I18N_DISCLAIMER_TEXT,
	LPC_I18N_DISCLAIMER_TITLE,
	LPC_I18N_DONE,
	LPC_I18N_DONE_LIST_1,
	LPC_I18N_DONE_LIST_2,
	LPC_I18N_DONE_LIST_3,
	LPC_I18N_DONE_LIST_4,
	LPC_I18N_DONE_NEXT,
	LPC_I18N_DONE_TEXT,
	LPC_I18N_DONE_TITLE,
	LPC_I18N_EXIT,
	LPC_I18N_FIRMWARE_ERROR_INTERNET,
	LPC_I18N_FIRMWARE_ERROR_NETGEAR,
	LPC_I18N_FIRMWARE_ERROR_SUPPORT,
	LPC_I18N_FIRMWARE_TEXT,
	LPC_I18N_FIRMWARE_TITLE,
	LPC_I18N_INTRO_SUBTITLE,
	LPC_I18N_INTRO_TEXT_1,
	LPC_I18N_INTRO_TEXT_2,
	LPC_I18N_INTRO_TITLE,
	LPC_I18N_MANAGE_ABOUT,
	LPC_I18N_MANAGE_ABOUT_DESCRIPTION,
	LPC_I18N_MANAGE_ACCOUNT,
	LPC_I18N_MANAGE_CHANGE,
	LPC_I18N_MANAGE_DISABLED,
	LPC_I18N_MANAGE_NO_NETWORK,
	LPC_I18N_MANAGE_PARENTALCONTROLS,
	LPC_I18N_MANAGE_PARENTALCONTROLS_DISABLED,
	LPC_I18N_MANAGE_PARENTALCONTROLS_ENABLED,
	LPC_I18N_MANAGE_ROUTER,
	LPC_I18N_MANAGE_ROUTER_MODEL,
	LPC_I18N_MANAGE_SETTINGS_BUNDLE,
	LPC_I18N_MANAGE_SETTINGS_BUNDLE_CUSTOM,
	LPC_I18N_MANAGE_SETTINGS_BUNDLE_CUSTOM_LABEL,
	LPC_I18N_MANAGE_SETTINGS_BUNDLE_HIGH,
	LPC_I18N_MANAGE_SETTINGS_BUNDLE_LOW,
	LPC_I18N_MANAGE_SETTINGS_BUNDLE_MINIMAL,
	LPC_I18N_MANAGE_SETTINGS_BUNDLE_MODERATE,
	LPC_I18N_MANAGE_SETTINGS_BUNDLE_NONE,
	LPC_I18N_MANAGE_SETTINGS_LINK,
	LPC_I18N_MANAGE_STATUS,
	LPC_I18N_MANAGE_STATUS_CHECKING,
	LPC_I18N_MANAGE_STATUS_DISABLING,
	LPC_I18N_MANAGE_STATUS_ENABLING,
	LPC_I18N_MANAGE_TITLE,
	LPC_I18N_MANAGE_TOGGLE,
	LPC_I18N_MANAGE_TOGGLE_DISABLED,
	LPC_I18N_MANAGE_TOGGLE_ENABLED,
    LPC_I18N_MANAGE_BYPASS_ACCOUNT, //yankai add
    LPC_I18N_MANAGE_BYPASS_ENTER, //yankai add
	LPC_I18N_MANAGE_BYPASS_LOGOUT, //
	LPC_I18N_NEXT,
	LPC_I18N_OK,
	LPC_I18N_PRESIGNIN_ACCOUNT_CREATE,
	LPC_I18N_PRESIGNIN_ACCOUNT_EXISTING,
	LPC_I18N_PRESIGNIN_SUBTITLE_1,
	LPC_I18N_PRESIGNIN_SUBTITLE_2,
	LPC_I18N_PRESIGNIN_TITLE,
	LPC_I18N_PREV,
	LPC_I18N_REFRESH,
	LPC_I18N_ROUTERLOGIN_PASSWORD,
	LPC_I18N_ROUTERLOGIN_PROMPT,
	LPC_I18N_ROUTERLOGIN_TITLE,
	LPC_I18N_ROUTERLOGIN_USERNAME,
	LPC_I18N_SETTINGS_BUNDLE_HIGH,
	LPC_I18N_SETTINGS_BUNDLE_HIGH_DESCRIPTION,
	LPC_I18N_SETTINGS_BUNDLE_LOW,
	LPC_I18N_SETTINGS_BUNDLE_LOW_DESCRIPTION,
	LPC_I18N_SETTINGS_BUNDLE_MINIMAL,
	LPC_I18N_SETTINGS_BUNDLE_MINIMAL_DESCRIPTION,
	LPC_I18N_SETTINGS_BUNDLE_MODERATE,
	LPC_I18N_SETTINGS_BUNDLE_MODERATE_DESCRIPTION,
	LPC_I18N_SETTINGS_BUNDLE_NONE,
	LPC_I18N_SETTINGS_BUNDLE_NONE_DESCRIPTION,
	LPC_I18N_SETTINGS_BUNDLE_CUSTOM,
	LPC_I18N_SETTINGS_BUNDLE_CUSTOM_DESCRIPTION, ///kai.yan add
	LPC_I18N_SETTINGS_ERROR,
	LPC_I18N_SETTINGS_STATUS_GET,
	LPC_I18N_SETTINGS_STATUS_SET,
	LPC_I18N_SETTINGS_SUBTITLE,
	LPC_I18N_SETTINGS_TITLE,
	LPC_I18N_SIGNIN_ERROR_FAILED,
	LPC_I18N_SIGNIN_ERROR_SIGNIN,
	LPC_I18N_SIGNIN_FORGOT,
	LPC_I18N_SIGNIN_PASSWORD,
	LPC_I18N_SIGNIN_STATUS,
	LPC_I18N_SIGNIN_SUBTITLE,
	LPC_I18N_SIGNIN_TITLE,
	LPC_I18N_SIGNIN_USERNAME,
	LPC_I18N_TITLE,
	LPC_I18N_TRYAGAIN,
	LPC_I18N_UPGRADE_ALERT,
	LPC_I18N_UPGRADE_ALERT_OK,
	LPC_I18N_UPGRADE_ALERT_TITLE,
	LPC_I18N_UPGRADE_CONFIRM,
	LPC_I18N_UPGRADE_CONFIRM_NO,
	LPC_I18N_UPGRADE_CONFIRM_TITLE,
	LPC_I18N_UPGRADE_CONFIRM_YES,
	LPC_I18N_WEBSITE_TEXT_1,
	LPC_I18N_WEBSITE_TEXT_2,
	LPC_I18N_GENERAL_STATUS_COMMMICATING,
	LPC_I18N_GENERAL_WAIT_TITLE,
	LPC_I18N_INTRO_LEARNMORE,
    LPC_I18N_BYPASS_STATUS_LOGIN, //kai.yan add
    LPC_I18N_BYPASS_STATUS_LOGOUT, //kai.yan add
	LPC_I18N_BYPASS_STATUS_USERLIST,
	LPC_I18N_BYPASS_ACCOUNT_TITLE,
	LPC_I18N_BYPASS_USERLIST_SUBTITLE,
	LPC_I18N_BYPASS_ERROR_USERLIST,
	LPC_I18N_BYPASS_REFLESH,
	LPC_I18N_BYPASS_BACK,
	LPC_I18N_BYPASS_LOGIN_SUBTITLE,
	LPC_I18N_BYPASS_ERROR_PWD,
	LPC_I18N_BYPASS_ERROR_LOGIN,
	LPC_I18N_BYPASS_USERNAME,
	LPC_I18N_BYPASS_PWD,
	LPC_I18N_BYPASS_CANCEL,
	LPC_I18N_BYPASS_LOGIN,
	LPC_I18N_BYPASS_LOGIN_MSG,
	LPC_I18N_BYPASS_LOGOUT,
	LPC_I18N_BYPASS_ERROR_NOUSER,
	LPC_I18N_SETTINGS_CUSTOM_TYPE0,
	LPC_I18N_SETTINGS_CUSTOM_TYPE1,
	LPC_I18N_SETTINGS_CUSTOM_TYPE2,
	LPC_I18N_SETTINGS_CUSTOM_TYPE3,
	LPC_I18N_SETTINGS_CUSTOM_TYPE4,
	LPC_I18N_SETTINGS_CUSTOM_TYPE5,
	LPC_I18N_SETTINGS_CUSTOM_TYPE6,
	LPC_I18N_SETTINGS_CUSTOM_TYPE7,
	LPC_I18N_SETTINGS_CUSTOM_TYPE8,
	LPC_I18N_SETTINGS_CUSTOM_TYPE9,
	LPC_I18N_SETTINGS_CUSTOM_TYPE10,
	LPC_I18N_SETTINGS_CUSTOM_TYPE11,
	LPC_I18N_SETTINGS_CUSTOM_TYPE12,
	LPC_I18N_SETTINGS_CUSTOM_TYPE13,
	LPC_I18N_SETTINGS_CUSTOM_TYPE14,
	LPC_I18N_SETTINGS_CUSTOM_TYPE15,
	LPC_I18N_SETTINGS_CUSTOM_TYPE16,
	LPC_I18N_SETTINGS_CUSTOM_TYPE17,
	LPC_I18N_SETTINGS_CUSTOM_TYPE18,
	LPC_I18N_SETTINGS_CUSTOM_TYPE19,
	LPC_I18N_SETTINGS_CUSTOM_TYPE20,
	LPC_I18N_SETTINGS_CUSTOM_TYPE21,
	LPC_I18N_SETTINGS_CUSTOM_TYPE22,
	LPC_I18N_SETTINGS_CUSTOM_TYPE23,
	LPC_I18N_SETTINGS_CUSTOM_TYPE24,
	LPC_I18N_SETTINGS_CUSTOM_TYPE25,
	LPC_I18N_SETTINGS_CUSTOM_TYPE26,
	LPC_I18N_SETTINGS_CUSTOM_TYPE27,
	LPC_I18N_SETTINGS_CUSTOM_TYPE28,
	LPC_I18N_SETTINGS_CUSTOM_TYPE29,
	LPC_I18N_SETTINGS_CUSTOM_TYPE30,
	LPC_I18N_SETTINGS_CUSTOM_TYPE31,
	LPC_I18N_SETTINGS_CUSTOM_TYPE32,
	LPC_I18N_SETTINGS_CUSTOM_TYPE33,
	LPC_I18N_SETTINGS_CUSTOM_TYPE34,
	LPC_I18N_SETTINGS_CUSTOM_TYPE35,
	LPC_I18N_SETTINGS_CUSTOM_TYPE36,
	LPC_I18N_SETTINGS_CUSTOM_TYPE37,
	LPC_I18N_SETTINGS_CUSTOM_TYPE38,
	LPC_I18N_SETTINGS_CUSTOM_TYPE39,
	LPC_I18N_SETTINGS_CUSTOM_TYPE40,
	LPC_I18N_SETTINGS_CUSTOM_TYPE41,
	LPC_I18N_SETTINGS_CUSTOM_TYPE42,
	LPC_I18N_SETTINGS_CUSTOM_TYPE43,
	LPC_I18N_SETTINGS_CUSTOM_TYPE44,
	LPC_I18N_SETTINGS_CUSTOM_TYPE45,
	LPC_I18N_SETTINGS_CUSTOM_TYPE46,
	LPC_I18N_SETTINGS_CUSTOM_TYPE47,
	LPC_I18N_SETTINGS_CUSTOM_TYPE48,
	LPC_I18N_SETTINGS_CUSTOM_TYPE49,
	LPC_I18N_SETTINGS_CUSTOM_TYPE50,
	LPC_I18N_SETTINGS_CUSTOM_TYPE51,
	LPC_I18N_SETTINGS_CUSTOM_TYPE52,
	LPC_I18N_SETTINGS_CUSTOM_TYPE53,
	LPC_I18N_SETTINGS_CUSTOM_TYPE54,
	LPC_I18N_SETTINGS_CUSTOM_TYPE55,
	LPC_I18N_SETTINGS_CUSTOM_TYPE56,
	LPC_I18N_SETTINGS_CUSTOM_TYPE57,
	LPC_I18N_SETTINGS_CUSTOM_TYPE58,
	LPC_I18N_SETTINGS_CUSTOM_TYPE59,

};

struct I18N_BIND
{
	const char *code;
	int ident;
};

static const I18N_BIND g_i18nBind[] = {
	{ "abort", LPC_I18N_ABORT },
	{ "basic.back", LPC_I18N_BASIC_BACK },
	{ "cancel", LPC_I18N_CANCEL },
	{ "createaccount.check", LPC_I18N_CREATEACCOUNT_CHECK },
	{ "createaccount.check.available", LPC_I18N_CREATEACCOUNT_CHECK_AVAILABLE },
	{ "createaccount.check.unavailable", LPC_I18N_CREATEACCOUNT_CHECK_UNAVAILABLE },
	{ "createaccount.email", LPC_I18N_CREATEACCOUNT_EMAIL },
	{ "createaccount.email.confirm", LPC_I18N_CREATEACCOUNT_EMAIL_CONFIRM },
	{ "createaccount.email.invalid", LPC_I18N_CREATEACCOUNT_EMAIL_INVALID },
	{ "createaccount.email.taken", LPC_I18N_CREATEACCOUNT_EMAIL_TAKEN },
	{ "createaccount.email.taken.signin", LPC_I18N_CREATEACCOUNT_EMAIL_TAKEN_SIGNIN },
	{ "createaccount.error", LPC_I18N_CREATEACCOUNT_ERROR },
	{ "createaccount.error.device", LPC_I18N_CREATEACCOUNT_ERROR_DEVICE },
	{ "createaccount.error.password", LPC_I18N_CREATEACCOUNT_ERROR_PASSWORD },
	{ "createaccount.error.username", LPC_I18N_CREATEACCOUNT_ERROR_USERNAME },
	{ "createaccount.password", LPC_I18N_CREATEACCOUNT_PASSWORD },
	{ "createaccount.password.confirm", LPC_I18N_CREATEACCOUNT_PASSWORD_CONFIRM },
	{ "createaccount.status", LPC_I18N_CREATEACCOUNT_STATUS },
	{ "createaccount.subtitle", LPC_I18N_CREATEACCOUNT_SUBTITLE },
	{ "createaccount.title", LPC_I18N_CREATEACCOUNT_TITLE },
	{ "createaccount.username", LPC_I18N_CREATEACCOUNT_USERNAME },
	{ "custom.start", LPC_I18N_CUSTOM_START },
	{ "custom.subtitle", LPC_I18N_CUSTOM_SUBTITLE },
	{ "custom.text.1", LPC_I18N_CUSTOM_TEXT_1 },
	{ "custom.text.2", LPC_I18N_CUSTOM_TEXT_2 },
	{ "custom.title", LPC_I18N_CUSTOM_TITLE },
	{ "device.api.create", LPC_I18N_DEVICE_API_CREATE },
	{ "device.api.get", LPC_I18N_DEVICE_API_GET },
	{ "device.error.device_id_not_mine", LPC_I18N_DEVICE_ERROR_DEVICE_ID_NOT_MINE },
	{ "device.error.device_key", LPC_I18N_DEVICE_ERROR_DEVICE_KEY },
	{ "device.error.netgear", LPC_I18N_DEVICE_ERROR_NETGEAR },
	{ "device.router.get", LPC_I18N_DEVICE_ROUTER_GET },
	{ "device.router.init", LPC_I18N_DEVICE_ROUTER_INIT },
	{ "device.router.set", LPC_I18N_DEVICE_ROUTER_SET },
	{ "disclaimer.text", LPC_I18N_DISCLAIMER_TEXT },
	{ "disclaimer.title", LPC_I18N_DISCLAIMER_TITLE },
	{ "done", LPC_I18N_DONE },
	{ "done.list.1", LPC_I18N_DONE_LIST_1 },
	{ "done.list.2", LPC_I18N_DONE_LIST_2 },
	{ "done.list.3", LPC_I18N_DONE_LIST_3 },
	{ "done.list.4", LPC_I18N_DONE_LIST_4 },
	{ "done.next", LPC_I18N_DONE_NEXT },
	{ "done.text", LPC_I18N_DONE_TEXT },
	{ "done.title", LPC_I18N_DONE_TITLE },
	{ "exit", LPC_I18N_EXIT },
	{ "firmware.error.internet", LPC_I18N_FIRMWARE_ERROR_INTERNET },
	{ "firmware.error.netgear", LPC_I18N_FIRMWARE_ERROR_NETGEAR },
	{ "firmware.error.support", LPC_I18N_FIRMWARE_ERROR_SUPPORT },
	{ "firmware.text", LPC_I18N_FIRMWARE_TEXT },
	{ "firmware.title", LPC_I18N_FIRMWARE_TITLE },
	{ "intro.subtitle", LPC_I18N_INTRO_SUBTITLE },
	{ "intro.text.1", LPC_I18N_INTRO_TEXT_1 },
	{ "intro.text.2", LPC_I18N_INTRO_TEXT_2 },
	{ "intro.title", LPC_I18N_INTRO_TITLE },
	{ "manage.about", LPC_I18N_MANAGE_ABOUT },
	{ "manage.about.description", LPC_I18N_MANAGE_ABOUT_DESCRIPTION },
	{ "manage.account", LPC_I18N_MANAGE_ACCOUNT },
	{ "manage.change", LPC_I18N_MANAGE_CHANGE },
	{ "manage.disabled", LPC_I18N_MANAGE_DISABLED },
	{ "manage.no_network", LPC_I18N_MANAGE_NO_NETWORK },
	{ "manage.parentalcontrols", LPC_I18N_MANAGE_PARENTALCONTROLS },
	{ "manage.parentalcontrols.disabled", LPC_I18N_MANAGE_PARENTALCONTROLS_DISABLED },
	{ "manage.parentalcontrols.enabled", LPC_I18N_MANAGE_PARENTALCONTROLS_ENABLED },
	{ "manage.router", LPC_I18N_MANAGE_ROUTER },
	{ "manage.router.model", LPC_I18N_MANAGE_ROUTER_MODEL },
	{ "manage.settings.bundle", LPC_I18N_MANAGE_SETTINGS_BUNDLE },
	{ "manage.settings.bundle.custom", LPC_I18N_MANAGE_SETTINGS_BUNDLE_CUSTOM },
	{ "manage.settings.bundle.custom.label", LPC_I18N_MANAGE_SETTINGS_BUNDLE_CUSTOM_LABEL },
	{ "manage.settings.bundle.high", LPC_I18N_MANAGE_SETTINGS_BUNDLE_HIGH },
	{ "manage.settings.bundle.low", LPC_I18N_MANAGE_SETTINGS_BUNDLE_LOW },
	{ "manage.settings.bundle.minimal", LPC_I18N_MANAGE_SETTINGS_BUNDLE_MINIMAL },
	{ "manage.settings.bundle.moderate", LPC_I18N_MANAGE_SETTINGS_BUNDLE_MODERATE },
	{ "manage.settings.bundle.none", LPC_I18N_MANAGE_SETTINGS_BUNDLE_NONE },
	{ "manage.settings.link", LPC_I18N_MANAGE_SETTINGS_LINK },
	{ "manage.status", LPC_I18N_MANAGE_STATUS },
	{ "manage.status.checking", LPC_I18N_MANAGE_STATUS_CHECKING },
	{ "manage.status.disabling", LPC_I18N_MANAGE_STATUS_DISABLING },
	{ "manage.status.enabling", LPC_I18N_MANAGE_STATUS_ENABLING },
	{ "manage.title", LPC_I18N_MANAGE_TITLE },
	{ "manage.toggle", LPC_I18N_MANAGE_TOGGLE },
	{ "manage.toggle.disabled", LPC_I18N_MANAGE_TOGGLE_DISABLED },
	{ "manage.toggle.enabled", LPC_I18N_MANAGE_TOGGLE_ENABLED },
    { "manage.bypass.account", LPC_I18N_MANAGE_BYPASS_ACCOUNT},  //yankai add
    { "manage.bypass.enter", LPC_I18N_MANAGE_BYPASS_ENTER}, //yankai add
	{ "magage.bypass.logout",LPC_I18N_MANAGE_BYPASS_LOGOUT},
	{ "next", LPC_I18N_NEXT },
	{ "ok", LPC_I18N_OK },
	{ "presignin.account.create", LPC_I18N_PRESIGNIN_ACCOUNT_CREATE },
	{ "presignin.account.existing", LPC_I18N_PRESIGNIN_ACCOUNT_EXISTING },
	{ "presignin.subtitle.1", LPC_I18N_PRESIGNIN_SUBTITLE_1 },
	{ "presignin.subtitle.2", LPC_I18N_PRESIGNIN_SUBTITLE_2 },
	{ "presignin.title", LPC_I18N_PRESIGNIN_TITLE },
	{ "prev", LPC_I18N_PREV },
	{ "refresh", LPC_I18N_REFRESH },
	{ "routerlogin.password", LPC_I18N_ROUTERLOGIN_PASSWORD },
	{ "routerlogin.prompt", LPC_I18N_ROUTERLOGIN_PROMPT },
	{ "routerlogin.title", LPC_I18N_ROUTERLOGIN_TITLE },
	{ "routerlogin.username", LPC_I18N_ROUTERLOGIN_USERNAME },
	{ "settings.bundle.high", LPC_I18N_SETTINGS_BUNDLE_HIGH },
	{ "settings.bundle.high.description", LPC_I18N_SETTINGS_BUNDLE_HIGH_DESCRIPTION },
	{ "settings.bundle.low", LPC_I18N_SETTINGS_BUNDLE_LOW },
	{ "settings.bundle.low.description", LPC_I18N_SETTINGS_BUNDLE_LOW_DESCRIPTION },
	{ "settings.bundle.minimal", LPC_I18N_SETTINGS_BUNDLE_MINIMAL },
	{ "settings.bundle.minimal.description", LPC_I18N_SETTINGS_BUNDLE_MINIMAL_DESCRIPTION },
	{ "settings.bundle.moderate", LPC_I18N_SETTINGS_BUNDLE_MODERATE },
	{ "settings.bundle.moderate.description", LPC_I18N_SETTINGS_BUNDLE_MODERATE_DESCRIPTION },
	{ "settings.bundle.none", LPC_I18N_SETTINGS_BUNDLE_NONE },
	{ "settings.bundle.none.description", LPC_I18N_SETTINGS_BUNDLE_NONE_DESCRIPTION },
	{ "settings.bundle.custom", LPC_I18N_SETTINGS_BUNDLE_CUSTOM },
	{ "settings.bundle.custom.description", LPC_I18N_SETTINGS_BUNDLE_CUSTOM_DESCRIPTION },///kai.yan add
	{ "settings.error", LPC_I18N_SETTINGS_ERROR },
	{ "settings.status.get", LPC_I18N_SETTINGS_STATUS_GET },
	{ "settings.status.set", LPC_I18N_SETTINGS_STATUS_SET },
	{ "settings.subtitle", LPC_I18N_SETTINGS_SUBTITLE },
	{ "settings.title", LPC_I18N_SETTINGS_TITLE },
	{ "signin.error.failed", LPC_I18N_SIGNIN_ERROR_FAILED },
	{ "signin.error.signin", LPC_I18N_SIGNIN_ERROR_SIGNIN },
	{ "signin.forgot", LPC_I18N_SIGNIN_FORGOT },
	{ "signin.password", LPC_I18N_SIGNIN_PASSWORD },
	{ "signin.status", LPC_I18N_SIGNIN_STATUS },
	{ "signin.subtitle", LPC_I18N_SIGNIN_SUBTITLE },
	{ "signin.title", LPC_I18N_SIGNIN_TITLE },
	{ "signin.username", LPC_I18N_SIGNIN_USERNAME },
	{ "title", LPC_I18N_TITLE },
	{ "tryagain", LPC_I18N_TRYAGAIN },
	{ "upgrade.alert", LPC_I18N_UPGRADE_ALERT },
	{ "upgrade.alert.ok", LPC_I18N_UPGRADE_ALERT_OK },
	{ "upgrade.alert.title", LPC_I18N_UPGRADE_ALERT_TITLE },
	{ "upgrade.confirm", LPC_I18N_UPGRADE_CONFIRM },
	{ "upgrade.confirm.no", LPC_I18N_UPGRADE_CONFIRM_NO },
	{ "upgrade.confirm.title", LPC_I18N_UPGRADE_CONFIRM_TITLE },
	{ "upgrade.confirm.yes", LPC_I18N_UPGRADE_CONFIRM_YES },
	{ "website.text.1", LPC_I18N_WEBSITE_TEXT_1 },
	{ "website.text.2", LPC_I18N_WEBSITE_TEXT_2 },
	{ "general.status.commmicating", LPC_I18N_GENERAL_STATUS_COMMMICATING },
	{ "general.wait.title", LPC_I18N_GENERAL_WAIT_TITLE },
	{ "intro.learnmore", LPC_I18N_INTRO_LEARNMORE },
    { "bypass.status.login", LPC_I18N_BYPASS_STATUS_LOGIN }, //kai.yan add
    { "bypass.status.logout", LPC_I18N_BYPASS_STATUS_LOGOUT },//kai.yan
	{ "bypass.status.userlist", LPC_I18N_BYPASS_STATUS_USERLIST },
	{ "bypass.account.title", LPC_I18N_BYPASS_ACCOUNT_TITLE },
	{ "bypass.userlist.subtitle", LPC_I18N_BYPASS_USERLIST_SUBTITLE },
	{ "bypass.error.userlist", LPC_I18N_BYPASS_ERROR_USERLIST },
	{ "bypass.reflesh", LPC_I18N_BYPASS_REFLESH },
	{ "bypass.back", LPC_I18N_BYPASS_BACK },
	{ "bypass.login.subtitle", LPC_I18N_BYPASS_LOGIN_SUBTITLE },
	{ "bypass.error.password", LPC_I18N_BYPASS_ERROR_PWD },
	{ "bypass.error.login", LPC_I18N_BYPASS_ERROR_LOGIN },
	{ "bypass.username", LPC_I18N_BYPASS_USERNAME },
	{ "bypass.password", LPC_I18N_BYPASS_PWD },
	{ "bypass.cancel", LPC_I18N_BYPASS_CANCEL },
	{ "bypass.login", LPC_I18N_BYPASS_LOGIN },
	{ "bypass.login.msg", LPC_I18N_BYPASS_LOGIN_MSG },
	{ "bypass.logout", LPC_I18N_BYPASS_LOGOUT },
	{ "bypass.error.nouser", LPC_I18N_BYPASS_ERROR_NOUSER},
	{ "bundle.custom.type0", LPC_I18N_SETTINGS_CUSTOM_TYPE0}, ///bundle custom type
	{ "bundle.custom.type1", LPC_I18N_SETTINGS_CUSTOM_TYPE1},
	{ "bundle.custom.type2", LPC_I18N_SETTINGS_CUSTOM_TYPE2},
	{ "bundle.custom.type3", LPC_I18N_SETTINGS_CUSTOM_TYPE3},
	{ "bundle.custom.type4", LPC_I18N_SETTINGS_CUSTOM_TYPE4},
	{ "bundle.custom.type5", LPC_I18N_SETTINGS_CUSTOM_TYPE5},
	{ "bundle.custom.type6", LPC_I18N_SETTINGS_CUSTOM_TYPE6},
	{ "bundle.custom.type7", LPC_I18N_SETTINGS_CUSTOM_TYPE7},
	{ "bundle.custom.type8", LPC_I18N_SETTINGS_CUSTOM_TYPE8},
	{ "bundle.custom.type9", LPC_I18N_SETTINGS_CUSTOM_TYPE9},
	{ "bundle.custom.type10", LPC_I18N_SETTINGS_CUSTOM_TYPE10},
	{ "bundle.custom.type11", LPC_I18N_SETTINGS_CUSTOM_TYPE11},
	{ "bundle.custom.type12", LPC_I18N_SETTINGS_CUSTOM_TYPE12},
	{ "bundle.custom.type13", LPC_I18N_SETTINGS_CUSTOM_TYPE13},
	{ "bundle.custom.type14", LPC_I18N_SETTINGS_CUSTOM_TYPE14},
	{ "bundle.custom.type15", LPC_I18N_SETTINGS_CUSTOM_TYPE15},
	{ "bundle.custom.type16", LPC_I18N_SETTINGS_CUSTOM_TYPE16},
	{ "bundle.custom.type17", LPC_I18N_SETTINGS_CUSTOM_TYPE17},
	{ "bundle.custom.type18", LPC_I18N_SETTINGS_CUSTOM_TYPE18},
	{ "bundle.custom.type19", LPC_I18N_SETTINGS_CUSTOM_TYPE19},
	{ "bundle.custom.type20", LPC_I18N_SETTINGS_CUSTOM_TYPE20},
	{ "bundle.custom.type21", LPC_I18N_SETTINGS_CUSTOM_TYPE21},
	{ "bundle.custom.type22", LPC_I18N_SETTINGS_CUSTOM_TYPE22},
	{ "bundle.custom.type23", LPC_I18N_SETTINGS_CUSTOM_TYPE23},
	{ "bundle.custom.type24", LPC_I18N_SETTINGS_CUSTOM_TYPE24},
	{ "bundle.custom.type25", LPC_I18N_SETTINGS_CUSTOM_TYPE25},
	{ "bundle.custom.type26", LPC_I18N_SETTINGS_CUSTOM_TYPE26},
	{ "bundle.custom.type27", LPC_I18N_SETTINGS_CUSTOM_TYPE27},
	{ "bundle.custom.type28", LPC_I18N_SETTINGS_CUSTOM_TYPE28},
	{ "bundle.custom.type29", LPC_I18N_SETTINGS_CUSTOM_TYPE29},
	{ "bundle.custom.type30", LPC_I18N_SETTINGS_CUSTOM_TYPE30},
	{ "bundle.custom.type31", LPC_I18N_SETTINGS_CUSTOM_TYPE31},
	{ "bundle.custom.type32", LPC_I18N_SETTINGS_CUSTOM_TYPE32},
	{ "bundle.custom.type33", LPC_I18N_SETTINGS_CUSTOM_TYPE33},
	{ "bundle.custom.type34", LPC_I18N_SETTINGS_CUSTOM_TYPE34},
	{ "bundle.custom.type35", LPC_I18N_SETTINGS_CUSTOM_TYPE35},
	{ "bundle.custom.type36", LPC_I18N_SETTINGS_CUSTOM_TYPE36},
	{ "bundle.custom.type37", LPC_I18N_SETTINGS_CUSTOM_TYPE37},
	{ "bundle.custom.type38", LPC_I18N_SETTINGS_CUSTOM_TYPE38},
	{ "bundle.custom.type39", LPC_I18N_SETTINGS_CUSTOM_TYPE39},
	{ "bundle.custom.type40", LPC_I18N_SETTINGS_CUSTOM_TYPE40},
	{ "bundle.custom.type41", LPC_I18N_SETTINGS_CUSTOM_TYPE41},
	{ "bundle.custom.type42", LPC_I18N_SETTINGS_CUSTOM_TYPE42},
	{ "bundle.custom.type43", LPC_I18N_SETTINGS_CUSTOM_TYPE43},
	{ "bundle.custom.type44", LPC_I18N_SETTINGS_CUSTOM_TYPE44},
	{ "bundle.custom.type45", LPC_I18N_SETTINGS_CUSTOM_TYPE45},
	{ "bundle.custom.type46", LPC_I18N_SETTINGS_CUSTOM_TYPE46},
	{ "bundle.custom.type47", LPC_I18N_SETTINGS_CUSTOM_TYPE47},
	{ "bundle.custom.type48", LPC_I18N_SETTINGS_CUSTOM_TYPE48},
	{ "bundle.custom.type49", LPC_I18N_SETTINGS_CUSTOM_TYPE49},
	{ "bundle.custom.type50", LPC_I18N_SETTINGS_CUSTOM_TYPE50},
	{ "bundle.custom.type51", LPC_I18N_SETTINGS_CUSTOM_TYPE51},
	{ "bundle.custom.type52", LPC_I18N_SETTINGS_CUSTOM_TYPE52},
	{ "bundle.custom.type53", LPC_I18N_SETTINGS_CUSTOM_TYPE53},
	{ "bundle.custom.type54", LPC_I18N_SETTINGS_CUSTOM_TYPE54},
	{ "bundle.custom.type55", LPC_I18N_SETTINGS_CUSTOM_TYPE55},
	{ "bundle.custom.type56", LPC_I18N_SETTINGS_CUSTOM_TYPE56},
	{ "bundle.custom.type57", LPC_I18N_SETTINGS_CUSTOM_TYPE57},
	{ "bundle.custom.type58", LPC_I18N_SETTINGS_CUSTOM_TYPE58},
	{ "bundle.custom.type59", LPC_I18N_SETTINGS_CUSTOM_TYPE59},
	
};

#define BBT_MODE_CLICKHERE "<span style=\" color:#40a900;\">%1</span>"
#define BBT_MODE_GOOD "<span style=\" color:#303030;\">%1</span>"\
	" <span style=\" color:#40a900;\">%2</span>"

LPCPlugin::LPCPlugin()
	: m_mainFrame(NULL)
	, m_pendingRestart(false)
{
	Q_INIT_RESOURCE(resources);
	DBGLOGINIT();

	for (size_t i = 0; i < sizeof(g_i18nBind) / sizeof(g_i18nBind[0]); i++) {
		m_i18nMap.insert(g_i18nBind[i].code, g_i18nBind[i].ident);
	}

	m_uuid = QUuid("CED77229-C59A-4669-9C26-0BB4860D96A7");
	m_jumpTimer.setSingleShot(true);
	connect(&m_jumpTimer, SIGNAL(timeout()), SLOT(restartSlot()));
}

LPCPlugin::~LPCPlugin()
{
	if (m_mainFrame) {
		m_mainFrame->detachEnv();
		m_mainFrame = NULL;
	}
	DBGLOG(LOG_INFO, 1, QString::fromUtf8("Exited"));
	DBGLOGTERM();
}

QVariant LPCPlugin::command(int cmd,const QVariant &paras)
{
	DBGLOG(LOG_INFO, 1, QString::fromUtf8("command %1").arg(cmd));
	if(cmd ==I2CMD_UUID)
	{
		return QString(PLUGIN_UUID);
	}
	else if(cmd==I2CMD_VERSIONNUMBER)
	{
		return QString(VERSION_STRING);
	}
	else if(cmd==I2CMD_TIMETO_CREATEUI)
	{
		m_mainFrame = new LPCMainFrame(this,NULL);
		GENIE2_REGISTER_UI(m_mainFrame,"1300:1650:Parental_Controls:cmd");
		GENIE2_SUBSCRIB_MESSAGE(SCCMD_ROUTER_MODEL);
		GENIE2_SUBSCRIB_MESSAGE(SCCMD_CONNECTION_STATE);
	}
	else if(cmd==I2CMD_TIMETO_START)
	{
	}
	else if(cmd ==I2CMD_UI_ACTION)
	{
		if (m_mainFrame) {
			m_mainFrame->onActivate(true);
		}
	}
	else if(cmd==I2CMD_UI_LOWER)
	{
		if (m_mainFrame) {
			m_mainFrame->onActivate(false);
		}
	}
	else if(cmd ==I2CMD_NOTIFY_MESSAGE)
	{
		QVariantList list = paras.toList();
		if(list.size() == 2)
		{
			int  msgtype = list.at(0).toInt();
			if(msgtype==SCCMD_ROUTER_MODEL)
			{
				static QString routetype="";
				QString currentroutetype = list.at(1).toString();
				if(currentroutetype.size() ==0 || (routetype.size() > 0 && routetype.compare(currentroutetype) == 0))
				{
					routetype = currentroutetype;
					restart();
				}
			}
			else if (msgtype==SCCMD_CONNECTION_STATE)
			{
				int status = list.at(1).toInt();
				if(status == 1)
				{
					restart();
				}
			}
		}
	}
	else if(cmd==EPCC_SMARTNETWORK_DATA)
	{
		DBGLOG(LOG_INFO, 1, QString::fromUtf8("command EPCC_SMARTNETWORK_DATA"));
		QVariantList lst=paras.toList();
		const int c=lst.count();
		Q_ASSERT(c>0);
		bool isRemote=lst[0].toBool();
		QString loginUser=(c>1)?lst[1].toString():QString();
		QString loginPwd=(c>2)?lst[2].toString():QString();
		QString cpName=(c>3)?lst[3].toString():QString();
		QString cpUser=(c>4)?lst[4].toString():QString();
		QString cpPwd=(c>5)?lst[5].toString():QString();
		if (m_mainFrame) {
			DBGLOG(LOG_INFO, 1, QString::fromUtf8("command EPCC_SMARTNETWORK_DATA jumpStart"));
			//QMessageBox::information(NULL, cpName, QString::fromLatin1("%1 %2 %3 %4").arg(loginUser).arg(loginPwd).arg(cpUser).arg(cpPwd));
			m_mainFrame->jumpSmart(isRemote ? cpName : QString(), loginUser, loginPwd, cpUser, cpPwd);
		} else {
			DBGLOG(LOG_INFO, 1, QString::fromUtf8("command EPCC_SMARTNETWORK_DATA m_mainFrame == NULL!!!!"));
		}
	}
	return QVariant();
}

QByteArray LPCPlugin::currentLanguageCode()
{
	QByteArray lang("enu");
	return lang;
}

void LPCPlugin::reportLPCStatus(int status)
{
	if (status < LPCSTATUS_UNKNOWN || status > LPCSTATUS_LEVEL_NONE) {
		status = LPCSTATUS_UNKNOWN;

		QString statusText;

		switch (status) {
		case LPCSTATUS_UNKNOWN:
		case LPCSTATUS_CHECKING:
		case LPCSTATUS_ERROR:
			GENIE2_UI_COMMAND(UICMD_BIGBUTTION_BUTLINE_ERROR,LPC_I18N_CLICK);
			break;
		case LPCSTATUS_DISABLED:
			GENIE2_UI_COMMAND(UICMD_BIGBUTTION_BUTLINE_GOOD,LPC_I18N_STATUS_NAME_DISABLED);
			break;
		default:
			GENIE2_UI_COMMAND(UICMD_BIGBUTTION_BUTLINE_GOOD,LPC_I18N_STATUS_NAME_ENABLED);
			break;
		}
	}
}

void LPCPlugin::changeLanguage()
{
}

QString LPCPlugin::translateText(const char *code)
{
	QString result;
	QMap<QByteArray, int>::const_iterator it = m_i18nMap.find(code);
	if (it != m_i18nMap.end()) {
		result = GENIE2_GET_TEXT(it.value());
	}
	return result;
}

void LPCPlugin::queryRouterAuth(QString& username, QString& password)
{
	QVariant vUsername = GENIE2_GET_SETTING(PLUGIN_ROUTECONF_USER);
	QVariant vPassword = GENIE2_GET_SETTING(PLUGIN_ROUTECONF_PWD);
	if (vUsername.isValid() && vPassword.isValid()) {
		username = vUsername.toString();
		password = vPassword.toString();
	}
}

void LPCPlugin::reportLPCEnableDisable(bool enable)
{
    GENIE2_STATISTICS(QString::fromLatin1(enable ? "P1" : "P2"));
}

void LPCPlugin::reportLPCLevel(int level)
{
	static const char *levelName[] = {
		"PL2", "PL3", "PL4", "PL5", "PL6", "PL1",
	};
	if (level >= 0 && level < 6) {
        GENIE2_STATISTICS(QString::fromLatin1(levelName[level]));
	}
}

void LPCPlugin::reportLPCBasicSettingClick()
{
	GENIE2_STATISTICS("P3");
}

void LPCPlugin::reportLPCCustomSettingClick()
{
	GENIE2_STATISTICS("P4");
}

QString LPCPlugin::querySmartNetworkBaseUrl()
{
	return GENIE2_GET_SETTING(SETTING_FCML_LINK).toString();
}

void LPCPlugin::restart()
{
	if (!m_pendingRestart) {
		m_pendingRestart = true;
		m_jumpTimer.start(5000);
	}
}

void LPCPlugin::restartSlot()
{
	if (m_pendingRestart) {
		if (m_mainFrame) {
			m_mainFrame->navigateTo("firmware");
		}
		m_pendingRestart = false;
	}
}


