#include "TripResultCache.h"

QString TripResultCache::makeKey(const QString &fromCode, const QString &toCode,
                                  const QString &mode, const QString &dateTimeIso)
{
    // "now" mode deliberately ignores the timestamp, since it changes every
    // call - repeat taps within the TTL should still hit the same entry.
    if (mode == QLatin1String("now"))
        return fromCode + QLatin1Char('|') + toCode + QLatin1String("|now");

    return fromCode + QLatin1Char('|') + toCode + QLatin1Char('|') + mode + QLatin1Char('|') + dateTimeIso;
}

void TripResultCache::store(const QString &key, const QByteArray &rawJson)
{
    m_entries.insert(key, {rawJson, QDateTime::currentDateTime()});
}

QByteArray TripResultCache::lookup(const QString &key) const
{
    const auto it = m_entries.constFind(key);
    if (it == m_entries.constEnd())
        return QByteArray();

    if (it->storedAt.secsTo(QDateTime::currentDateTime()) > kTtlSeconds)
        return QByteArray();

    return it->rawJson;
}
