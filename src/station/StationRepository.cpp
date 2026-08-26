#include "StationRepository.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QRegularExpression>
#include <algorithm>

namespace {

QString normalize(const QString &s)
{
    QString decomposed = s.normalized(QString::NormalizationForm_D);
    static const QRegularExpression combiningMarks(QStringLiteral("\\p{Mn}"));
    decomposed.remove(combiningMarks);
    return decomposed.toLower();
}

// 0 = no match, higher = better match, for a single name variant.
int matchScore(const QString &normalizedName, const QString &normalizedQuery)
{
    if (normalizedQuery.isEmpty())
        return 0;
    if (normalizedName == normalizedQuery)
        return 3;
    if (normalizedName.startsWith(normalizedQuery))
        return 2;
    if (normalizedName.contains(normalizedQuery))
        return 1;
    return 0;
}

bool isIntercityType(const QString &stationType)
{
    return stationType.contains(QStringLiteral("INTERCITY"));
}

}

bool StationRepository::load(const QString &bundledPath, const QString &refreshedPath)
{
    if (!refreshedPath.isEmpty() && QFile::exists(refreshedPath) && loadFromFile(refreshedPath))
        return true;

    return loadFromFile(bundledPath);
}

bool StationRepository::reloadFromRefreshed(const QString &refreshedPath)
{
    return loadFromFile(refreshedPath);
}

bool StationRepository::loadFromFile(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
        return false;

    const QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    if (!doc.isObject())
        return false;

    const QJsonArray payload = doc.object().value(QStringLiteral("payload")).toArray();
    if (payload.isEmpty())
        return false;

    QVector<StationRecord> parsed;
    parsed.reserve(payload.count());

    for (const QJsonValue &value : payload) {
        const QJsonObject obj = value.toObject();
        const QJsonObject namen = obj.value(QStringLiteral("namen")).toObject();

        StationRecord record;
        record.code = obj.value(QStringLiteral("code")).toString();
        record.land = obj.value(QStringLiteral("land")).toString();
        record.nameLong = namen.value(QStringLiteral("lang")).toString();
        record.nameMedium = namen.value(QStringLiteral("middel")).toString();
        record.nameShort = namen.value(QStringLiteral("kort")).toString();
        record.stationType = obj.value(QStringLiteral("stationType")).toString();

        if (record.code.isEmpty() || record.nameLong.isEmpty())
            continue;

        parsed.append(record);
    }

    if (parsed.isEmpty())
        return false;

    m_stations = parsed;
    return true;
}

QVector<StationRecord> StationRepository::search(const QString &query, const QString &countryCode, int limit) const
{
    const QString normalizedQuery = normalize(query.trimmed());

    struct Scored {
        const StationRecord *record;
        int score;
    };
    QVector<Scored> scored;
    scored.reserve(m_stations.count());

    for (const StationRecord &record : m_stations) {
        if (!countryCode.isEmpty() && record.land.compare(countryCode, Qt::CaseInsensitive) != 0)
            continue;

        const int score = std::max({
            matchScore(normalize(record.nameLong), normalizedQuery),
            matchScore(normalize(record.nameMedium), normalizedQuery),
            matchScore(normalize(record.nameShort), normalizedQuery),
        });

        if (normalizedQuery.isEmpty() || score > 0)
            scored.append({&record, score});
    }

    std::sort(scored.begin(), scored.end(), [](const Scored &a, const Scored &b) {
        if (a.score != b.score)
            return a.score > b.score;
        const bool aIntercity = isIntercityType(a.record->stationType);
        const bool bIntercity = isIntercityType(b.record->stationType);
        if (aIntercity != bIntercity)
            return aIntercity;
        return a.record->nameLong.compare(b.record->nameLong, Qt::CaseInsensitive) < 0;
    });

    QVector<StationRecord> results;
    results.reserve(std::min(limit, static_cast<int>(scored.count())));
    for (int i = 0; i < scored.count() && i < limit; ++i)
        results.append(*scored.at(i).record);

    return results;
}
