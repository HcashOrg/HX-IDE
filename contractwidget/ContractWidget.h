#ifndef CONTRACTWIDGET_H
#define CONTRACTWIDGET_H

#include <QWidget>

namespace Ui {
class ContractWidget;
}
class QTreeWidgetItem;
class ContractWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ContractWidget(QWidget *parent = 0);
    ~ContractWidget();
public slots:
    void RefreshTree();
private slots:
    void ContractClicked(QTreeWidgetItem *item, int column);
private:
    void InitWidget();
private:
    Ui::ContractWidget *ui;
};

#endif // CONTRACTWIDGET_H
