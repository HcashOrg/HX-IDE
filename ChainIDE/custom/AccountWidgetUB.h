#ifndef AccountWidgetUB_H
#define AccountWidgetUB_H

#include <QDialog>
#include "popwidget/MoveableDialog.h"

namespace Ui {
class AccountWidgetUB;
}
class QTreeWidgetItem;
class AccountWidgetUB : public MoveableDialog
{
    Q_OBJECT

public:
    explicit AccountWidgetUB(QWidget *parent = 0);
    ~AccountWidgetUB();
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
    Ui::AccountWidgetUB *ui;
    class DataPrivate;
    DataPrivate *_p;
};

#endif // AccountWidgetUB_H
