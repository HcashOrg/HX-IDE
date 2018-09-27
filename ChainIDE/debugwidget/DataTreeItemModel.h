#ifndef DataTreeItemModel_h__
#define DataTreeItemModel_h__

#include <QAbstractItemModel>

//////////////////////////////////////////////////////////////////////////
///<summary> 通用树model </summary>
///
///<remarks> 朱正天,2017/11/2. </remarks>
///////////////////////////////////////////////////////////////////////////*/
class BaseItemData;
class BaseTreeItem;
typedef std::shared_ptr<BaseItemData> BaseItemDataPtr;

class DataTreeItemModel : public QAbstractItemModel
{
	Q_OBJECT

public:
    DataTreeItemModel(QObject *parent = nullptr);
	~DataTreeItemModel();
public:
    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    bool removeRows(int row, int count, const QModelIndex & parent = QModelIndex()) override;
    bool insertRows(int row, int count, const QModelIndex & parent = QModelIndex()) override;
public:
    QStringList mimeTypes()const override;
    Qt::DropActions supportedDropActions() const override;
    QMimeData * mimeData(const QModelIndexList &indexes) const override;
    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) override;

public:
	//////////////////////////////////////////////////////////////////////////
	///<summary>  初始化编组树数据.</summary>
	///
	///<remarks> 朱正天,2017/1/8.</remarks>
	///////////////////////////////////////////////////////////////////////////
    void InitModelData(BaseItemDataPtr data);
private:
    void ConvertDataToItem(BaseItemDataPtr data, BaseTreeItem *&rootItem);
private:
	class DataTreeItemModelPrivate;
    DataTreeItemModelPrivate *_p;
};
#endif // DataTreeItemModel_h__
