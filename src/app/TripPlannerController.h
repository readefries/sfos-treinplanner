#pragma once

#include <QDateTime>
#include <QObject>
#include <QString>

#include "../cache/TripResultCache.h"
#include "../station/StationRepository.h"

class ApiKeyManager;
class NetworkStateMonitor;
class NsApiClient;
class RequestBudgetTracker;
class StationBootstrap;
class StationListModel;
class TripResultsModel;

// Ties the station/trip search flow together for QML: station search is
// always local (StationRepository, no network); trip search goes through
// the offline/cache/budget checks described in the plan before ever
// touching NsApiClient.
class TripPlannerController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool searching READ isSearching NOTIFY searchingChanged)
    Q_PROPERTY(QString lastErrorMessage READ lastErrorMessage NOTIFY lastErrorMessageChanged)
    Q_PROPERTY(bool isOnline READ isOnline NOTIFY onlineChanged)

public:
    explicit TripPlannerController(QObject *parent = nullptr);

    StationListModel *stationListModel() const { return m_stationListModel; }
    TripResultsModel *tripResultsModel() const { return m_tripResultsModel; }
    ApiKeyManager *apiKeyManager() const { return m_apiKeyManager; }
    RequestBudgetTracker *requestBudgetTracker() const { return m_budgetTracker; }

    bool isSearching() const { return m_searching; }
    QString lastErrorMessage() const { return m_lastErrorMessage; }
    bool isOnline() const;

    // mode is one of "now" / "departAt" / "arriveBy". explicitDateTime is
    // ignored for "now" (recomputed fresh on every call, including refresh).
    Q_INVOKABLE void searchStations(const QString &query, const QString &countryCode = QStringLiteral("NL"));
    Q_INVOKABLE void planTrip(const QString &fromCode, const QString &toCode,
                               const QString &mode, const QDateTime &explicitDateTime);
    Q_INVOKABLE void refreshTrip();

signals:
    void searchingChanged();
    void lastErrorMessageChanged();
    void onlineChanged();
    void tripResultsReady();
    void noUsableApiKeyError();

private:
    void executePlanTrip(bool bypassCache);
    QDateTime resolveSearchDateTime() const;
    void setSearching(bool searching);
    void setLastErrorMessage(const QString &message);

    StationRepository m_stationRepository;
    StationListModel *m_stationListModel;
    TripResultsModel *m_tripResultsModel;
    ApiKeyManager *m_apiKeyManager;
    RequestBudgetTracker *m_budgetTracker;
    NetworkStateMonitor *m_networkMonitor;
    NsApiClient *m_apiClient;
    StationBootstrap *m_stationBootstrap;
    TripResultCache m_tripCache;

    QString m_lastFromCode;
    QString m_lastToCode;
    QString m_lastMode;
    QDateTime m_lastExplicitDateTime;
    QString m_pendingCacheKey;

    bool m_searching = false;
    QString m_lastErrorMessage;
};
