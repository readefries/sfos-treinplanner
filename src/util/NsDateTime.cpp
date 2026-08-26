#include "NsDateTime.h"

#include <QRegularExpression>

namespace NsDateTime {

QString toIso8601LocalOffset(const QDateTime &localDateTime)
{
    const QString datePart = localDateTime.toString(QStringLiteral("yyyy-MM-ddTHH:mm:ss"));

    const int offsetSeconds = localDateTime.offsetFromUtc();
    const int offsetMinutesTotal = offsetSeconds / 60;
    const QChar sign = offsetMinutesTotal >= 0 ? QLatin1Char('+') : QLatin1Char('-');
    const int absMinutes = qAbs(offsetMinutesTotal);

    const QString offsetPart = QStringLiteral("%1%2%3")
        .arg(sign)
        .arg(absMinutes / 60, 2, 10, QLatin1Char('0'))
        .arg(absMinutes % 60, 2, 10, QLatin1Char('0'));

    return datePart + offsetPart;
}

QDateTime parseTolerant(const QString &value)
{
    static const QRegularExpression offsetNoColon(QStringLiteral("([+-]\\d{2})(\\d{2})$"));
    static const QRegularExpression offsetWithColon(QStringLiteral("[+-]\\d{2}:\\d{2}$"));

    QString normalized = value;
    if (!offsetWithColon.match(normalized).hasMatch()) {
        normalized.replace(offsetNoColon, QStringLiteral("\\1:\\2"));
    }

    return QDateTime::fromString(normalized, Qt::ISODate);
}

}
