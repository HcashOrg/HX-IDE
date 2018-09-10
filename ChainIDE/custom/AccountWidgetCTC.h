#ifndef ACCOUNTWIDGETCTC_H
#define ACCOUNTWIDGETCTC_H

#include "popwidget/MoveableDialog.h"

namespace Ui {
class AccountWidgetCTC;
}
class QTreeWidgetItem;
class AccountWidgetCTC : public MoveableDialog
{
    Q_OBJECT

public:
    explicit AccountWidgetCTC(QWidget *parent = 0);
    ~AccountWidgetCTC();
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
    Ui::AccountWidgetCTC *ui;
    class DataPrivate;
    DataPrivate *_p;
};

#endif // ACCOUNTWIDGETCTC_H
