#include "NetworkStateMonitor.h"

#include <QNetworkConfigurationManager>

NetworkStateMonitor::NetworkStateMonitor(QObject *parent)
    : QObject(parent)
    , m_manager(new QNetworkConfigurationManager(this))
{
    connect(m_manager, &QNetworkConfigurationManager::onlineStateChanged,
            this, &NetworkStateMonitor::isOnlineChanged);
}

bool NetworkStateMonitor::isOnline() const
{
    return m_manager->isOnline();
}
