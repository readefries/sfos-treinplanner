#include "NsApiClient.h"

#include "../network/RequestBudgetTracker.h"
#include "../settings/ApiKeyManager.h"

#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>

const QString NsApiClient::kBaseUrl = QStringLiteral("https://gateway.apiportal.ns.nl/reisinformatie-api");

NsApiClient::NsApiClient(ApiKeyManager *apiKeyManager, RequestBudgetTracker *budgetTracker, QObject *parent)
    : QObject(parent)
    , m_apiKeyManager(apiKeyManager)
    , m_budgetTracker(budgetTracker)
{
}

void NsApiClient::sendRequest(const QString &path, const QUrlQuery &query,
                               const std::function<void(QNetworkReply *)> &onFinished)
{
    const QString key = m_apiKeyManager->effectiveKey();
    if (key.isEmpty()) {
        emit noUsableApiKey();
        return;
    }

    QUrl url(kBaseUrl + path);
    url.setQuery(query);

    QNetworkRequest request(url);
    request.setRawHeader("Subscription-Key", key.toUtf8());

    m_budgetTracker->recordRequest();

    QNetworkReply *reply = m_manager.get(request);
    connect(reply, &QNetworkReply::finished, this, [reply, onFinished]() {
        onFinished(reply);
        reply->deleteLater();
    });
}

void NsApiClient::planTrip(const QString &fromStationCode, const QString &toStationCode,
                            const QString &dateTimeIso, bool departure)
{
    QUrlQuery query;
    query.addQueryItem(QStringLiteral("fromStation"), fromStationCode);
    query.addQueryItem(QStringLiteral("toStation"), toStationCode);
    query.addQueryItem(QStringLiteral("dateTime"), dateTimeIso);
    query.addQueryItem(QStringLiteral("departure"), departure ? QStringLiteral("true") : QStringLiteral("false"));

    sendRequest(QStringLiteral("/api/v3/trips"), query, [this](QNetworkReply *reply) {
        if (reply->error() != QNetworkReply::NoError) {
            emit tripPlanFailed(reply->errorString());
            return;
        }
        emit tripPlanReceived(reply->readAll());
    });
}

void NsApiClient::fetchAllStations()
{
    sendRequest(QStringLiteral("/api/v2/stations"), QUrlQuery(), [this](QNetworkReply *reply) {
        if (reply->error() != QNetworkReply::NoError) {
            emit stationsFetchFailed(reply->errorString());
            return;
        }
        emit stationsFetched(reply->readAll());
    });
}
