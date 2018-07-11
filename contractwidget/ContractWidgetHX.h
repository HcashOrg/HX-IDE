#ifndef CONTRACTWIDGET_H
#define CONTRACTWIDGET_H

#include <QWidget>

namespace Ui {
class ContractWidgetHX;
}
class QTreeWidgetItem;
class ContractWidgetHX : public QWidget
{
    Q_OBJECT

public:
    explicit ContractWidgetHX(QWidget *parent = 0);
    ~ContractWidgetHX();
public slots:
    void RefreshTree();
    void retranslator();
private slots:
    void ContractClicked(QTreeWidgetItem *item, QTreeWidgetItem *itempre);
private:
    void InitWidget();
    void InitTree();
private:
    Ui::ContractWidgetHX *ui;
};

#endif // CONTRACTWIDGET_H
