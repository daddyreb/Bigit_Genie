#include "QGenieQRCode.h"
#include "qrencode.h"
#include <QDebug>


QGenieQRCode::QGenieQRCode(const QString &s)
{
    qDebug()<<"QGenieQRCode::QGenieQRCode()";
    QByteArray array=s.toLocal8Bit();
	char *data=array.data();
    mQRCode=(void *)QRcode_encodeString
            (data,0,QR_ECLEVEL_M, QR_MODE_8,1);
    //mQRCode=(void *)QRcode_encodeData
    //        (s.length(),(unsigned char *)data,0,QR_ECLEVEL_M);
}

QGenieQRCode::~QGenieQRCode()
{
    QRcode_free((QRcode *)mQRCode);
}

const unsigned char *QGenieQRCode::data() const
{
    if(!mQRCode)
    {
        return 0;
    }
    else
    {
        return ((QRcode *)mQRCode)->data;
    }
}

int QGenieQRCode::width() const
{
    if(!mQRCode)
    {
        return 0;
    }
    else
    {
        return ((QRcode *)mQRCode)->width;
    }
}
