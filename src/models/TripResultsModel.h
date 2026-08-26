#pragma once

#include <QAbstractListModel>
#include <QDateTime>
#include <QVariant>
#include <QVariantList>
#include <QVector>

struct TripLegSummary {
    QString categoryShort;
    QString number;
    QString direction;
    QString originName;
    QString destinationName;
    QString plannedTrack;
    QString actualTrack;
    bool cancelled = false;
};

struct TripSummary {
    QDateTime plannedDeparture;
    QDateTime actualDeparture;
    QDateTime plannedArrival;
    QDateTime actualArrival;
    int departureDelayMinutes = 0;
    int arrivalDelayMinutes = 0;
    int transfers = 0;
    QString status;
    bool cancelled = false;
    QVector<TripLegSummary> legs;
};

class TripResultsModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Role {
        PlannedDepartureRole = Qt::UserRole + 1,
        ActualDepartureRole,
        PlannedArrivalRole,
        ActualArrivalRole,
        DepartureDelayMinutesRole,
        ArrivalDelayMinutesRole,
        TransfersRole,
        StatusRole,
        CancelledRole,
        LegsRole,
    };

    explicit TripResultsModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    void setTrips(const QVector<TripSummary> &trips);
    void clear();
    Q_INVOKABLE QVariantMap get(int row) const;

    // Parses a raw /api/v3/trips response body into TripSummary records.
    static QVector<TripSummary> parseResponse(const QByteArray &rawJson);

private:
    QVector<TripSummary> m_trips;
};
