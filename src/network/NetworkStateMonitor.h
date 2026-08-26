#pragma once

#include <QObject>

QT_BEGIN_NAMESPACE
class QNetworkConfigurationManager;
QT_END_NAMESPACE

// Reports whether the device currently has a usable network connection, so
// TripPlannerController can short-circuit to an offline state without
// spending a request attempt. Sailfish OS ships Qt 5.6, where
// QNetworkConfigurationManager is the supported way to observe this
// (it predates Qt6's QNetworkInformation).
class NetworkStateMonitor : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isOnline READ isOnline NOTIFY isOnlineChanged)

public:
    explicit NetworkStateMonitor(QObject *parent = nullptr);

    bool isOnline() const;

signals:
    void isOnlineChanged();

private:
    QNetworkConfigurationManager *m_manager;
};
