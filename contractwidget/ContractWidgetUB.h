#ifndef ContractWidgetUB_H
#define ContractWidgetUB_H

#include <QWidget>

namespace Ui {
class ContractWidgetUB;
}
class QTreeWidgetItem;
class ContractWidgetUB : public QWidget
{
    Q_OBJECT

public:
    explicit ContractWidgetUB(QWidget *parent = 0);
    ~ContractWidgetUB();
public slots:
    void RefreshTree();
    void retranslator();
private slots:
    void ContractClicked(QTreeWidgetItem *item, QTreeWidgetItem *itempre);
private:
    void InitWidget();
private:
    Ui::ContractWidgetUB *ui;
};

#endif // ContractWidgetUB_H
