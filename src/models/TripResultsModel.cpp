#include "TripResultsModel.h"

#include "../util/NsDateTime.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

namespace {

int delayMinutesFromSeconds(const QJsonValue &secondsValue, const QDateTime &planned, const QDateTime &actual)
{
    if (secondsValue.isDouble())
        return qRound(secondsValue.toDouble() / 60.0);
    if (planned.isValid() && actual.isValid())
        return qRound(planned.secsTo(actual) / 60.0);
    return 0;
}

TripLegSummary parseLeg(const QJsonObject &legObj)
{
    TripLegSummary leg;

    const QJsonObject product = legObj.value(QStringLiteral("product")).toObject();
    leg.categoryShort = product.value(QStringLiteral("shortCategoryName")).toString();
    leg.number = product.value(QStringLiteral("number")).toString();
    leg.direction = legObj.value(QStringLiteral("direction")).toString();
    leg.cancelled = legObj.value(QStringLiteral("cancelled")).toBool()
        || legObj.value(QStringLiteral("partCancelled")).toBool();

    const QJsonObject origin = legObj.value(QStringLiteral("origin")).toObject();
    const QJsonObject destination = legObj.value(QStringLiteral("destination")).toObject();
    leg.originName = origin.value(QStringLiteral("name")).toString();
    leg.destinationName = destination.value(QStringLiteral("name")).toString();
    leg.plannedTrack = origin.value(QStringLiteral("plannedTrack")).toString();
    leg.actualTrack = origin.value(QStringLiteral("actualTrack")).toString();

    return leg;
}

TripSummary parseTrip(const QJsonObject &tripObj)
{
    TripSummary trip;
    trip.transfers = tripObj.value(QStringLiteral("transfers")).toInt();
    trip.status = tripObj.value(QStringLiteral("status")).toString();

    const QJsonArray legs = tripObj.value(QStringLiteral("legs")).toArray();
    trip.legs.reserve(legs.count());
    for (const QJsonValue &legValue : legs)
        trip.legs.append(parseLeg(legValue.toObject()));

    trip.cancelled = trip.status == QStringLiteral("CANCELLED");
    for (const TripLegSummary &leg : trip.legs)
        trip.cancelled = trip.cancelled || leg.cancelled;

    if (legs.isEmpty())
        return trip;

    const QJsonObject firstLeg = legs.first().toObject();
    const QJsonObject lastLeg = legs.last().toObject();
    const QJsonObject firstOrigin = firstLeg.value(QStringLiteral("origin")).toObject();
    const QJsonObject lastDestination = lastLeg.value(QStringLiteral("destination")).toObject();

    trip.plannedDeparture = NsDateTime::parseTolerant(firstOrigin.value(QStringLiteral("plannedDateTime")).toString());
    trip.actualDeparture = NsDateTime::parseTolerant(firstOrigin.value(QStringLiteral("actualDateTime")).toString());
    trip.plannedArrival = NsDateTime::parseTolerant(lastDestination.value(QStringLiteral("plannedDateTime")).toString());
    trip.actualArrival = NsDateTime::parseTolerant(lastDestination.value(QStringLiteral("actualDateTime")).toString());

    const QJsonArray firstLegStops = firstLeg.value(QStringLiteral("stops")).toArray();
    const QJsonArray lastLegStops = lastLeg.value(QStringLiteral("stops")).toArray();

    const QJsonValue departureDelaySeconds = firstLegStops.isEmpty()
        ? QJsonValue()
        : firstLegStops.first().toObject().value(QStringLiteral("departureDelayInSeconds"));
    const QJsonValue arrivalDelaySeconds = lastLegStops.isEmpty()
        ? QJsonValue()
        : lastLegStops.last().toObject().value(QStringLiteral("arrivalDelayInSeconds"));

    trip.departureDelayMinutes = delayMinutesFromSeconds(departureDelaySeconds, trip.plannedDeparture, trip.actualDeparture);
    trip.arrivalDelayMinutes = delayMinutesFromSeconds(arrivalDelaySeconds, trip.plannedArrival, trip.actualArrival);

    return trip;
}

QVariantList legsToVariantList(const QVector<TripLegSummary> &legs)
{
    QVariantList list;
    list.reserve(legs.count());
    for (const TripLegSummary &leg : legs) {
        QVariantMap map;
        map[QStringLiteral("category")] = leg.categoryShort;
        map[QStringLiteral("number")] = leg.number;
        map[QStringLiteral("direction")] = leg.direction;
        map[QStringLiteral("originName")] = leg.originName;
        map[QStringLiteral("destinationName")] = leg.destinationName;
        map[QStringLiteral("plannedTrack")] = leg.plannedTrack;
        map[QStringLiteral("actualTrack")] = leg.actualTrack;
        map[QStringLiteral("cancelled")] = leg.cancelled;
        list.append(map);
    }
    return list;
}

}

TripResultsModel::TripResultsModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int TripResultsModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_trips.count();
}

QVariant TripResultsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_trips.count())
        return QVariant();

    const TripSummary &trip = m_trips.at(index.row());
    switch (role) {
    case PlannedDepartureRole: return trip.plannedDeparture;
    case ActualDepartureRole: return trip.actualDeparture;
    case PlannedArrivalRole: return trip.plannedArrival;
    case ActualArrivalRole: return trip.actualArrival;
    case DepartureDelayMinutesRole: return trip.departureDelayMinutes;
    case ArrivalDelayMinutesRole: return trip.arrivalDelayMinutes;
    case TransfersRole: return trip.transfers;
    case StatusRole: return trip.status;
    case CancelledRole: return trip.cancelled;
    case LegsRole: return legsToVariantList(trip.legs);
    default: return QVariant();
    }
}

QHash<int, QByteArray> TripResultsModel::roleNames() const
{
    return {
        {PlannedDepartureRole, "plannedDeparture"},
        {ActualDepartureRole, "actualDeparture"},
        {PlannedArrivalRole, "plannedArrival"},
        {ActualArrivalRole, "actualArrival"},
        {DepartureDelayMinutesRole, "departureDelayMinutes"},
        {ArrivalDelayMinutesRole, "arrivalDelayMinutes"},
        {TransfersRole, "transfers"},
        {StatusRole, "status"},
        {CancelledRole, "cancelled"},
        {LegsRole, "legs"},
    };
}

void TripResultsModel::setTrips(const QVector<TripSummary> &trips)
{
    beginResetModel();
    m_trips = trips;
    endResetModel();
}

void TripResultsModel::clear()
{
    setTrips({});
}

QVariantMap TripResultsModel::get(int row) const
{
    QVariantMap map;
    if (row < 0 || row >= m_trips.count())
        return map;

    const QHash<int, QByteArray> roles = roleNames();
    for (auto it = roles.constBegin(); it != roles.constEnd(); ++it)
        map[QString::fromUtf8(it.value())] = data(index(row), it.key());
    return map;
}

QVector<TripSummary> TripResultsModel::parseResponse(const QByteArray &rawJson)
{
    const QJsonDocument doc = QJsonDocument::fromJson(rawJson);
    if (!doc.isObject())
        return {};

    const QJsonArray trips = doc.object().value(QStringLiteral("trips")).toArray();
    QVector<TripSummary> parsed;
    parsed.reserve(trips.count());
    for (const QJsonValue &tripValue : trips)
        parsed.append(parseTrip(tripValue.toObject()));

    return parsed;
}
