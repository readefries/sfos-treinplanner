#include "StationListModel.h"

StationListModel::StationListModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int StationListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_results.count();
}

QVariant StationListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_results.count())
        return QVariant();

    const StationRecord &record = m_results.at(index.row());
    switch (role) {
    case CodeRole: return record.code;
    case NameLongRole: return record.nameLong;
    case NameMediumRole: return record.nameMedium;
    case NameShortRole: return record.nameShort;
    case StationTypeRole: return record.stationType;
    default: return QVariant();
    }
}

QHash<int, QByteArray> StationListModel::roleNames() const
{
    return {
        {CodeRole, "code"},
        {NameLongRole, "nameLong"},
        {NameMediumRole, "nameMedium"},
        {NameShortRole, "nameShort"},
        {StationTypeRole, "stationType"},
    };
}

void StationListModel::setResults(const QVector<StationRecord> &results)
{
    beginResetModel();
    m_results = results;
    endResetModel();
}

QVariantMap StationListModel::get(int row) const
{
    QVariantMap map;
    if (row < 0 || row >= m_results.count())
        return map;

    const StationRecord &record = m_results.at(row);
    map[QStringLiteral("code")] = record.code;
    map[QStringLiteral("nameLong")] = record.nameLong;
    map[QStringLiteral("nameMedium")] = record.nameMedium;
    map[QStringLiteral("nameShort")] = record.nameShort;
    map[QStringLiteral("stationType")] = record.stationType;
    return map;
}
