#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QUrlQuery>
#include <functional>

class ApiKeyManager;
class RequestBudgetTracker;

// Thin wrapper around the NS Reisinformatie API. Every outgoing request goes
// through sendRequest(), which is the single point where the
// Ocp-Apim-Subscription-Key header is attached and RequestBudgetTracker is updated -
// this keeps budget accounting correct no matter how many call sites exist.
class NsApiClient : public QObject
{
    Q_OBJECT

public:
    explicit NsApiClient(ApiKeyManager *apiKeyManager, RequestBudgetTracker *budgetTracker, QObject *parent = nullptr);

    // dateTimeIso must already be in the NS-expected "yyyy-MM-ddTHH:mm:ss+HHMM"
    // format (see NsDateTime::toIso8601LocalOffset).
    void planTrip(const QString &fromStationCode, const QString &toStationCode,
                  const QString &dateTimeIso, bool departure);

    // Fetches the full station list in one call (confirmed: empty q, no
    // limit, no countryCodes returns everything NS serves).
    void fetchAllStations();

signals:
    void tripPlanReceived(const QByteArray &rawJson);
    void tripPlanFailed(const QString &errorMessage);
    void stationsFetched(const QByteArray &rawJson);
    void stationsFetchFailed(const QString &errorMessage);
    void noUsableApiKey();

private:
    void sendRequest(const QString &path, const QUrlQuery &query,
                      const std::function<void(QNetworkReply *)> &onFinished);

    ApiKeyManager *m_apiKeyManager;
    RequestBudgetTracker *m_budgetTracker;
    QNetworkAccessManager m_manager;

    static const QString kBaseUrl;
};
