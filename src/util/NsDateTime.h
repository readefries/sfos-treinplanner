#pragma once

#include <QDateTime>
#include <QString>

namespace NsDateTime {

// Builds "yyyy-MM-ddTHH:mm:ss+HHMM" using the UTC offset valid on localDateTime's
// own date (not "now"'s offset), matching the NS API's expected request format.
QString toIso8601LocalOffset(const QDateTime &localDateTime);

// Parses NS API timestamps, tolerating both "+0200" and "+02:00" offset styles.
QDateTime parseTolerant(const QString &value);

}
