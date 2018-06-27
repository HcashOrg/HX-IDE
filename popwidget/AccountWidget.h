#ifndef ACCOUNTWIDGET_H
#define ACCOUNTWIDGET_H

#include <QDialog>
#include "MoveableDialog.h"

namespace Ui {
class AccountWidget;
}

class AccountWidget : public MoveableDialog
{
    Q_OBJECT

public:
    explicit AccountWidget(QWidget *parent = 0);
    ~AccountWidget();
private slots:
    void jsonDataUpdated(const QString &id,const QString &data);

    void on_newAccount_clicked();
    void on_closeBtn_clicked();
private:
    void InitWidget();
    void InitTree();
private:
    Ui::AccountWidget *ui;
};

#endif // ACCOUNTWIDGET_H
