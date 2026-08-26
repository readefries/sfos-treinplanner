#include "StationBootstrap.h"

#include "../network/NetworkStateMonitor.h"
#include "../network/RequestBudgetTracker.h"
#include "../nsapi/NsApiClient.h"

#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>

StationBootstrap::StationBootstrap(NsApiClient *apiClient, RequestBudgetTracker *budgetTracker,
                                    NetworkStateMonitor *networkMonitor, QString refreshedPath,
                                    QObject *parent)
    : QObject(parent)
    , m_apiClient(apiClient)
    , m_budgetTracker(budgetTracker)
    , m_networkMonitor(networkMonitor)
    , m_refreshedPath(std::move(refreshedPath))
{
    connect(m_apiClient, &NsApiClient::stationsFetched, this, [this](const QByteArray &rawJson) {
        QDir().mkpath(QFileInfo(m_refreshedPath).absolutePath());
        QFile file(m_refreshedPath);
        if (!file.open(QIODevice::WriteOnly)) {
            emit refreshFailed(file.errorString());
            return;
        }
        file.write(rawJson);
        emit refreshed();
    });
    connect(m_apiClient, &NsApiClient::stationsFetchFailed, this, &StationBootstrap::refreshFailed);
}

bool StationBootstrap::isStale() const
{
    const QFileInfo info(m_refreshedPath);
    if (!info.exists())
        return true;
    return info.lastModified().addDays(kStaleAfterDays) < QDateTime::currentDateTime();
}

void StationBootstrap::maybeRefresh()
{
    if (!isStale())
        return;
    if (!m_networkMonitor->isOnline())
        return;
    if (m_budgetTracker->remaining() < kMinBudgetHeadroom)
        return;

    m_apiClient->fetchAllStations();
}
