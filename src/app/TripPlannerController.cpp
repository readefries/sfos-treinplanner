#include "TripPlannerController.h"

#include "../models/StationListModel.h"
#include "../models/TripResultsModel.h"
#include "../network/NetworkStateMonitor.h"
#include "../network/RequestBudgetTracker.h"
#include "../nsapi/NsApiClient.h"
#include "../settings/ApiKeyManager.h"
#include "../station/StationBootstrap.h"
#include "../util/NsDateTime.h"

#include <QStandardPaths>
#include <sailfishapp.h>

namespace {
QString refreshedStationsPath()
{
    const QString dir = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    return dir + QStringLiteral("/stations_refreshed.json");
}
}

TripPlannerController::TripPlannerController(QObject *parent)
    : QObject(parent)
    , m_stationListModel(new StationListModel(this))
    , m_tripResultsModel(new TripResultsModel(this))
    , m_apiKeyManager(new ApiKeyManager(this))
    , m_budgetTracker(new RequestBudgetTracker(this))
    , m_networkMonitor(new NetworkStateMonitor(this))
    , m_apiClient(new NsApiClient(m_apiKeyManager, m_budgetTracker, this))
{
    const QString bundledPath = SailfishApp::pathTo(QStringLiteral("data/stations_nl.json")).toLocalFile();
    const QString refreshedPath = refreshedStationsPath();
    m_stationRepository.load(bundledPath, refreshedPath);

    m_stationBootstrap = new StationBootstrap(m_apiClient, m_budgetTracker, m_networkMonitor, refreshedPath, this);
    connect(m_stationBootstrap, &StationBootstrap::refreshed, this, [this, refreshedPath]() {
        m_stationRepository.reloadFromRefreshed(refreshedPath);
    });
    m_stationBootstrap->maybeRefresh();

    connect(m_apiClient, &NsApiClient::tripPlanReceived, this, [this](const QByteArray &rawJson) {
        m_tripCache.store(m_pendingCacheKey, rawJson);
        m_tripResultsModel->setTrips(TripResultsModel::parseResponse(rawJson));
        setSearching(false);
        setLastErrorMessage(QString());
        emit tripResultsReady();
    });
    connect(m_apiClient, &NsApiClient::tripPlanFailed, this, [this](const QString &errorMessage) {
        setSearching(false);
        setLastErrorMessage(errorMessage);
    });
    connect(m_apiClient, &NsApiClient::noUsableApiKey, this, [this]() {
        setSearching(false);
        setLastErrorMessage(tr("Add an NS API key in Settings to plan trips."));
        emit noUsableApiKeyError();
    });

    connect(m_networkMonitor, &NetworkStateMonitor::isOnlineChanged, this, &TripPlannerController::onlineChanged);
}

bool TripPlannerController::isOnline() const
{
    return m_networkMonitor->isOnline();
}

void TripPlannerController::setSearching(bool searching)
{
    if (m_searching == searching)
        return;
    m_searching = searching;
    emit searchingChanged();
}

void TripPlannerController::setLastErrorMessage(const QString &message)
{
    if (m_lastErrorMessage == message)
        return;
    m_lastErrorMessage = message;
    emit lastErrorMessageChanged();
}

void TripPlannerController::searchStations(const QString &query, const QString &countryCode)
{
    m_stationListModel->setResults(m_stationRepository.search(query, countryCode));
}

QDateTime TripPlannerController::resolveSearchDateTime() const
{
    if (m_lastMode == QLatin1String("now"))
        return QDateTime::currentDateTime();
    return m_lastExplicitDateTime;
}

void TripPlannerController::planTrip(const QString &fromCode, const QString &toCode,
                                      const QString &mode, const QDateTime &explicitDateTime)
{
    m_lastFromCode = fromCode;
    m_lastToCode = toCode;
    m_lastMode = mode;
    m_lastExplicitDateTime = explicitDateTime;
    executePlanTrip(false);
}

void TripPlannerController::refreshTrip()
{
    if (m_lastFromCode.isEmpty() || m_lastToCode.isEmpty())
        return;
    executePlanTrip(true);
}

void TripPlannerController::executePlanTrip(bool bypassCache)
{
    const QDateTime searchDateTime = resolveSearchDateTime();
    const QString dateTimeIso = NsDateTime::toIso8601LocalOffset(searchDateTime);
    const bool departure = m_lastMode != QLatin1String("arriveBy");
    const QString cacheKey = TripResultCache::makeKey(m_lastFromCode, m_lastToCode, m_lastMode, dateTimeIso);

    if (!bypassCache) {
        const QByteArray cached = m_tripCache.lookup(cacheKey);
        if (!cached.isEmpty()) {
            m_tripResultsModel->setTrips(TripResultsModel::parseResponse(cached));
            setLastErrorMessage(QString());
            emit tripResultsReady();
            return;
        }
    }

    if (!m_networkMonitor->isOnline()) {
        setLastErrorMessage(tr("No network connection."));
        return;
    }

    if (m_budgetTracker->shouldBlock()) {
        setLastErrorMessage(tr("Request budget exhausted for the next few minutes."));
        return;
    }

    m_pendingCacheKey = cacheKey;
    setSearching(true);
    setLastErrorMessage(QString());
    m_apiClient->planTrip(m_lastFromCode, m_lastToCode, dateTimeIso, departure);
}
