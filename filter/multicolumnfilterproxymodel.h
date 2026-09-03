#ifndef MULTICOLUMNFILTERPROXYMODEL_H
#define MULTICOLUMNFILTERPROXYMODEL_H

#include <qsortfilterproxymodel.h>
#include <qtmetamacros.h>

// Include this class if you want to filter more than 1 specific column

class MultiColumnFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit MultiColumnFilterProxyModel(QObject *parent = nullptr)
        : QSortFilterProxyModel(parent) {}

    void setFilterColumns(const QList<int> &columns);

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;

private:
    QList<int> m_columns;
};

#endif // MULTICOLUMNFILTERPROXYMODEL_H
