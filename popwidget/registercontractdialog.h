#ifndef REGISTERCONTRACTDIALOG_H
#define REGISTERCONTRACTDIALOG_H

#include <QDialog>
#include "MoveableDialog.h"

namespace Ui {
class RegisterContractDialog;
}

class RegisterContractDialog : public MoveableDialog
{
    Q_OBJECT

public:
    explicit RegisterContractDialog(QWidget *parent = 0);
    ~RegisterContractDialog();

private slots:
    void jsonDataUpdated(const QString &id,const QString &data);

    void on_okBtn_clicked();

    void on_cancelBtn_clicked();

    void on_closeBtn_clicked();
private:
    void InitWidget();
    void InitAccountAddress();
private:
    Ui::RegisterContractDialog *ui;
};

#endif // REGISTERCONTRACTDIALOG_H
