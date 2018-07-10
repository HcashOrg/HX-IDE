#ifndef ACCOUNTWIDGET_H
#define ACCOUNTWIDGET_H

#include <QDialog>
#include "MoveableDialog.h"

namespace Ui {
class AccountWidget;
}
class QTreeWidgetItem;
class AccountWidget : public MoveableDialog
{
    Q_OBJECT

public:
    explicit AccountWidget(QWidget *parent = 0);
    ~AccountWidget();
private slots:
    void jsonDataUpdated(const QString &id,const QString &data);

    void on_newAccount_clicked();
    void on_importKey_clicked();
    void on_closeBtn_clicked();

    void CopyAddr();
    void QuerySlots();
protected:
    bool eventFilter(QObject *o, QEvent *e);
private:
    void InitWidget();
    void InitTree();
    void InitContextMenu();
private:
    void selectNext(const QList<QTreeWidgetItem*>& listItem);
    bool isEqualList(const QList<QTreeWidgetItem*>& list1,const QList<QTreeWidgetItem*>& list2);
private:
    Ui::AccountWidget *ui;
    class DataPrivate;
    DataPrivate *_p;
};

#endif // ACCOUNTWIDGET_H
