#pragma once

#include <QString>
#include <QVector>

struct StationRecord {
    QString code;
    QString land;
    QString nameLong;
    QString nameMedium;
    QString nameShort;
    QString stationType;
};

// Loads the bundled/refreshed NS station list and answers explicit-submit
// search queries entirely in memory - no network call is made for this.
class StationRepository
{
public:
    // bundledPath is the read-only asset shipped with the app; refreshedPath
    // is an optional newer copy written by StationBootstrap. If refreshedPath
    // exists and parses, it is used in full instead of the bundled file.
    bool load(const QString &bundledPath, const QString &refreshedPath = QString());

    // Reloads from a freshly-written refreshed file (called after a
    // successful StationBootstrap run without restarting the app).
    bool reloadFromRefreshed(const QString &refreshedPath);

    int count() const { return m_stations.count(); }
    bool isEmpty() const { return m_stations.isEmpty(); }

    // query is matched case/diacritic-insensitively against all three name
    // variants, ranked exact > prefix > substring, then Intercity stations
    // first, then alphabetically. countryCode filters on the "land" field
    // (empty countryCode disables the filter).
    QVector<StationRecord> search(const QString &query, const QString &countryCode = QStringLiteral("NL"), int limit = 20) const;

private:
    bool loadFromFile(const QString &path);

    QVector<StationRecord> m_stations;
};
