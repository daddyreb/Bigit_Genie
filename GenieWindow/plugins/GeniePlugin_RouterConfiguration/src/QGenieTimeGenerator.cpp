#include "QGenieTimeGenerator.h"


QGenieTimeGenerator::QGenieTimeGenerator()
{
}

int QGenieTimeGenerator::wirelessSettingWaitTime(const QString &router)
{
    static const struct {const char *name;int time;} cT[]={
        {"CG3300",60},
        {"CGD24G",60},
        {"DG834",60},
        {"DG834G",60},
        {"DG834GT",60},
        {"DG834GV",60},
        {"DG834N",60},
        {"DG834PN",60},
        {"DGN1000_RN",60},
        {"DGN2000",60},
        {"DGN2200",60},
        {"DGN2200M",60},
        {"DGN3500",60},
        {"DGND3300",60},
        {"DM111P",60},
        {"DM111PSP",60},
        {"JWNR2000",60},
        {"MBM621",60},
        {"MBR1210",60},
        {"MBR624GU",60},
        {"MBRN3000",60},
        {"RP614",60},
        {"WGR612",60},
        {"WGR614",60},
        {"WGR614L",60},
        {"WGT624",60},
        {"WNR2000",60},
        {"WNDR3300",60},
        {"WNDR3400",60},
        {"WNDR3700",70},
        {"WNDR4700",70},
        {"DGND4000",70},
        {"WNDR37AV",60},
        {"WNR1000",60},
        {"WNR2200",60},
        {"WNR3500",60},
        {"WNR3500L",60},
        {"WNR612",60},
        {"WNR834B",60},
        {"WNR834M",60},
        {"WNR854T",60},
        {"WPN824",60},
        {"WPN824EXT",60},
{"WPN824N",60},
{"DGNB2100",70},
{"DGND3700",70},
{"WNB2100",70},
{"WNDR3800",70},
{"WNDR4000",70},
{"WNDR4500",90},
{"WNXR2000",70}
    };
    const int cNum=sizeof(cT)/sizeof(cT[0]);
    int max=0;
    for(int i=0;i<cNum;i++)
    {
        max=qMax(max,cT[i].time);
        if(0==router.left(strlen(cT[i].name)).compare(cT[i].name,Qt::CaseInsensitive) )
        {
            return cT[i].time;
        }
    };
    return max;

}

int QGenieTimeGenerator::gaccSettingWaitTime(const QString &router)
{
    static const struct {const char *name;int time;} cT[]={
        {"CG3300",60},
        {"CGD24G",60},
        {"DG834",60},
        {"DG834G",60},
        {"DG834GT",60},
        {"DG834GV",60},
        {"DG834N",60},
        {"DG834PN",60},
        {"DGN1000_RN",60},
        {"DGN2000",60},
        {"DGN2200",60},
        {"DGN2200M",60},
        {"DGN3500",60},
        {"DGND3300",60},
        {"DM111P",60},
        {"DM111PSP",60},
        {"JWNR2000",60},
        {"MBM621",60},
        {"MBR1210",60},
        {"MBR624GU",60},
        {"MBRN3000",60},
        {"RP614",60},
        {"WGR612",60},
        {"WGR614",60},
        {"WGR614L",60},
        {"WGT624",60},
        {"WNR2000",60},
        {"WNDR3300",60},
        {"WNDR3400",60},
        {"WNDR3700",70},
        {"WNDR37AV",60},
        {"WNR1000",60},
        {"WNR2200",60},
        {"WNR3500",60},
        {"WNR3500L",60},
        {"WNR612",60},
        {"WNR834B",60},
        {"WNR834M",60},
        {"WNR854T",60},
        {"WPN824",60},
        {"WPN824EXT",60},
        {"WPN824N",60},
        {"DGNB2100",70},
        {"DGND3700",70},
        {"WNB2100",70},
        {"WNDR3800",70},
        {"WNDR4000",70},
        {"WNDR4500",90},
        {"WNXR2000",70}
    };

    const int cNum=sizeof(cT)/sizeof(cT[0]);
    int max=0;
    for(int i=0;i<cNum;i++)
    {
        max=qMax(max,cT[i].time);
        if(0==router.left(strlen(cT[i].name)).compare(cT[i].name,Qt::CaseInsensitive) )
        {
            return cT[i].time;
        }
    };
    return max;
}
