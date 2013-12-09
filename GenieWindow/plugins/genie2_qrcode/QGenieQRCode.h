#ifndef QGENIEQRCODE_H
#define QGENIEQRCODE_H

#include <QtCore/qglobal.h>

#if defined(QRCODE_LIBRARY)
#  define QRCODESHARED_EXPORT Q_DECL_EXPORT
#else
#  define QRCODESHARED_EXPORT Q_DECL_IMPORT
#endif

class QRCODESHARED_EXPORT QGenieQRCode
{
public:
    QGenieQRCode(const QString &s);
    ~QGenieQRCode();
    const unsigned char *data() const;
    int width() const;
protected:
    void *mQRCode;
};

#endif // QGENIEQRCODE_H
