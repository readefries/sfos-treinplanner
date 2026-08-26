#pragma once

#include <QAbstractListModel>
#include <QVector>

#include "../station/StationRepository.h"

class StationListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Role {
        CodeRole = Qt::UserRole + 1,
        NameLongRole,
        NameMediumRole,
        NameShortRole,
        StationTypeRole,
    };

    explicit StationListModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    void setResults(const QVector<StationRecord> &results);
    Q_INVOKABLE QVariantMap get(int row) const;

private:
    QVector<StationRecord> m_results;
};
