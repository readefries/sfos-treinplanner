#pragma once

#include <QDateTime>
#include <QList>
#include <QObject>

// Tracks this device's own NS API request rate against the documented
// 300 requests / 5 minutes ceiling, so the app can warn/back off before
// actually hitting a 429. This only sees local usage - it cannot see other
// installs sharing the same default key (see ApiKeyManager).
class RequestBudgetTracker : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int remaining READ remaining NOTIFY usageChanged)
    Q_PROPERTY(bool nearLimit READ nearLimit NOTIFY usageChanged)

public:
    static constexpr int kWindowSeconds = 300;
    static constexpr int kWindowLimit = 300;
    static constexpr int kNearLimitThreshold = 30;

    explicit RequestBudgetTracker(QObject *parent = nullptr);

    // Call immediately before actually sending a request.
    void recordRequest();

    int remaining() const;
    bool nearLimit() const;
    Q_INVOKABLE bool shouldBlock() const;

signals:
    void usageChanged();

private:
    void prune() const;

    mutable QList<QDateTime> m_recentRequests;
};
