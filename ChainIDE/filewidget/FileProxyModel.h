#ifndef FILEPROXYMODEL_H
#define FILEPROXYMODEL_H

#include <QSortFilterProxyModel>

class FileProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit FileProxyModel(QObject *parent = nullptr);
    ~FileProxyModel();
protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;
    bool filterAcceptsColumn(int source_column, const QModelIndex &source_parent) const override;
};

#endif // FILEPROXYMODEL_H
