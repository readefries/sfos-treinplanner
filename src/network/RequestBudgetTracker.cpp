#include "RequestBudgetTracker.h"

RequestBudgetTracker::RequestBudgetTracker(QObject *parent)
    : QObject(parent)
{
}

void RequestBudgetTracker::prune() const
{
    const QDateTime cutoff = QDateTime::currentDateTime().addSecs(-kWindowSeconds);
    while (!m_recentRequests.isEmpty() && m_recentRequests.first() < cutoff)
        m_recentRequests.removeFirst();
}

void RequestBudgetTracker::recordRequest()
{
    prune();
    m_recentRequests.append(QDateTime::currentDateTime());
    emit usageChanged();
}

int RequestBudgetTracker::remaining() const
{
    prune();
    return qMax(0, kWindowLimit - m_recentRequests.count());
}

bool RequestBudgetTracker::nearLimit() const
{
    return remaining() <= kNearLimitThreshold;
}

bool RequestBudgetTracker::shouldBlock() const
{
    return remaining() <= 0;
}
