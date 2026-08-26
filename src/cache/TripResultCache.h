#pragma once

#include <QByteArray>
#include <QDateTime>
#include <QHash>
#include <QString>

// Not a real cache - just absorbs accidental duplicate searches (double
// taps, back-then-forward navigation) within a short window so they don't
// spend a request. Trip data is inherently realtime, so the TTL is short
// and an explicit "Refresh" always bypasses this.
class TripResultCache
{
public:
    static constexpr int kTtlSeconds = 45;

    static QString makeKey(const QString &fromCode, const QString &toCode,
                            const QString &mode, const QString &dateTimeIso);

    void store(const QString &key, const QByteArray &rawJson);

    // Returns an empty QByteArray if there is no entry or it has expired.
    QByteArray lookup(const QString &key) const;

private:
    struct Entry {
        QByteArray rawJson;
        QDateTime storedAt;
    };

    QHash<QString, Entry> m_entries;
};
