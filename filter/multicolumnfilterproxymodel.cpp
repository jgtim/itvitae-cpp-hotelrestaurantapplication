#include "multicolumnfilterproxymodel.h"

void MultiColumnFilterProxyModel::setFilterColumns(const QList<int> &columns)
{
    // This function should be called if you are implementing custom filtering
    beginFilterChange();

    m_columns = columns;

    endFilterChange(QSortFilterProxyModel::Direction::Rows);
}

bool MultiColumnFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    for (int col : m_columns) {
        QModelIndex index = sourceModel()->index(sourceRow, col, sourceParent);
        if (sourceModel()->data(index).toString().contains(filterRegularExpression()))
            return true;
    }
    return false;
}
