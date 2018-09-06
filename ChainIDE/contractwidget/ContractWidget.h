#ifndef CONTRACTWIDGET_H
#define CONTRACTWIDGET_H

#include <QWidget>
//合约显示界面，用于展示已注册合约
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
    void retranslator();
private slots:
    void ContractClicked(QTreeWidgetItem *current, QTreeWidgetItem *previous);
    void CopyAddr();
private:
    void InitWidget();
    void InitTree();
    void InitContextMenu();
protected:
    bool eventFilter(QObject *watched, QEvent *event);
private:
    Ui::ContractWidget *ui;
    class DataPrivate;
    DataPrivate *_p;
};

#endif // CONTRACTWIDGET_H
