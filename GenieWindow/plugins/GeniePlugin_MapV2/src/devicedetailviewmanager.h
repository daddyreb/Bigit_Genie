#ifndef DEVICEDETAILVIEWMANAGER_H
#define DEVICEDETAILVIEWMANAGER_H

#include <QObject>
#include <QList>

class DeviceDetailView;
class DeviceDetailViewFrame;

class Device;

class DeviceDetailViewManager : public QObject
{
    Q_OBJECT
public:
    explicit DeviceDetailViewManager(QWidget *detailViewParent , QObject *parent = 0);
    ~DeviceDetailViewManager();
protected:
    DeviceDetailView        *m_detailView;
    DeviceDetailViewFrame   *m_detailViewFrame;

public:
    void viewDeviceDetails(const Device &dev);
    void requestViewDeviceDetailsOver();

signals:
    void viewDetailsOver(const Device &dev ,bool modified);
    void translateText(int idx ,QString *text);
    void requestRouterImagePath(const QString &model,int imageRole,QString *imagePath);

public slots:
    void onNewData(const QList<Device *> &data);
    void raise();
};

#endif // DEVICEDETAILVIEWMANAGER_H
