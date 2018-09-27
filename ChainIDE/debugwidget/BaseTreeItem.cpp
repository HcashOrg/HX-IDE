#include "BaseTreeItem.h"

class BaseTreeItem::BaseTreeItemPrivate
{
public:
	BaseTreeItemPrivate()
		:_ItemData(QVariant())
		, _CheckState(Qt::Unchecked)
	{

	}
public:
	QList<BaseTreeItem*> _ChildItems;//下级
	QVariant _ItemData;//数据
	BaseTreeItem* _ParentItem;

	Qt::CheckState _CheckState;
};

BaseTreeItem::BaseTreeItem(QVariant data, BaseTreeItem *parent)
	:_p(new BaseTreeItemPrivate())
{
	_p->_ParentItem = parent;
	_p->_ItemData = data;
}


BaseTreeItem::~BaseTreeItem(void)
{
	qDeleteAll(_p->_ChildItems);
	delete _p;
}

void BaseTreeItem::appendChild(BaseTreeItem *child)
{
	_p->_ChildItems.append(child);
}

BaseTreeItem * BaseTreeItem::GetChild(int row)
{
	return _p->_ChildItems.at(row);
}

int BaseTreeItem::childCount() const
{
	return _p->_ChildItems.count();
}

QVariant BaseTreeItem::data() const
{
    return _p->_ItemData;
}

void BaseTreeItem::setData(const QVariant &var)
{
    _p->_ItemData = var;
}

int BaseTreeItem::RowInParent() const
{
	return _p->_ParentItem ? _p->_ParentItem->_p->_ChildItems.indexOf(const_cast<BaseTreeItem*>(this)) : 0;
}

BaseTreeItem * BaseTreeItem::GetParent()
{
	return _p->_ParentItem;
}

void BaseTreeItem::SetParent(BaseTreeItem *parentItem)
{
	_p->_ParentItem = parentItem;
}

bool BaseTreeItem::removeChild(BaseTreeItem *childItem)
{
	return _p->_ChildItems.removeOne(childItem);
}

void BaseTreeItem::SetCheckState(Qt::CheckState checkState, bool childEffect, bool parentEffect)
{
	_p->_CheckState = checkState;
	if (childEffect) updateChildCheckState(checkState);
	if (parentEffect) updateParentCheckState(checkState);
}

Qt::CheckState BaseTreeItem::GetCheckState() const
{
	return _p->_CheckState;
}

void BaseTreeItem::updateChildCheckState(Qt::CheckState checkState)
{
	if (_p->_ChildItems.isEmpty() || Qt::PartiallyChecked == checkState) return;
	for (int i = 0; i < _p->_ChildItems.size(); ++i)
	{
		_p->_ChildItems.at(i)->SetCheckState(checkState, true, false);
	}
}

void BaseTreeItem::updateParentCheckState(Qt::CheckState checkState)
{
	if (_p->_ParentItem == nullptr) return;
	int checkNumber = 0, uncheckNumber = 0, partialCheckNumber = 0;
	for (int i = 0; i < _p->_ParentItem->_p->_ChildItems.size(); ++i)
	{
		Qt::CheckState state = _p->_ParentItem->_p->_ChildItems.at(i)->GetCheckState();
		state == Qt::Unchecked ? ++uncheckNumber : (state == Qt::Checked ? ++ checkNumber : ++partialCheckNumber);
	}
	_p->_ParentItem->SetCheckState(partialCheckNumber == 0 ? 
								   (uncheckNumber == 0 ? Qt::Checked : (checkNumber == 0 ? Qt::Unchecked : Qt::PartiallyChecked)):
								   Qt::PartiallyChecked,false,true);
}

