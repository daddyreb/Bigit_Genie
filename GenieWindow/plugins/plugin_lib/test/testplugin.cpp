#include "testplugin.h"
#include "QGenieBasePluginLib.h"
#include "QMyWidget.h"

TestPlugin::TestPlugin()
{ 
    setUiEnable();
    setVersion("2.2.21.2");

    //do not set parent to widget here, because framework will set it later
    setWidget(new QMyWidget);
   // setPluginName("TestPlugin");
    setPluginName("1900");
    setPriority(100);
    setBigbuttonImgName("bbt_demo");
    setLanFname("LanguageDemo.txt");


//    setBigbuttonText("");
}

TestPlugin::~TestPlugin()
{

}

Q_EXPORT_PLUGIN2(TestPlugin, TestPlugin)
