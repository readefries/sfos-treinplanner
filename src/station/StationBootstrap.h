#pragma once

#include <QObject>
#include <QString>

class NsApiClient;
class RequestBudgetTracker;
class NetworkStateMonitor;

// Background refresh of the bundled station list. Costs exactly one request
// (see NsApiClient::fetchAllStations), so it only needs a staleness check
// and a healthy budget/connectivity guard - no throttled multi-request
// scheduling is required.
class StationBootstrap : public QObject
{
    Q_OBJECT

public:
    static constexpr int kStaleAfterDays = 7;
    static constexpr int kMinBudgetHeadroom = 50;

    StationBootstrap(NsApiClient *apiClient, RequestBudgetTracker *budgetTracker,
                      NetworkStateMonitor *networkMonitor, QString refreshedPath,
                      QObject *parent = nullptr);

    // No-ops if the refreshed copy is fresh enough, offline, or budget is low.
    void maybeRefresh();

signals:
    // refreshedPath now contains fresh data - caller should reload StationRepository from it.
    void refreshed();
    void refreshFailed(const QString &errorMessage);

private:
    bool isStale() const;

    NsApiClient *m_apiClient;
    RequestBudgetTracker *m_budgetTracker;
    NetworkStateMonitor *m_networkMonitor;
    QString m_refreshedPath;
};
