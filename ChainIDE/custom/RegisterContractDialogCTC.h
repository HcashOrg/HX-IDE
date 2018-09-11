#ifndef REGISTERCONTRACTDIALOGCTC_H
#define REGISTERCONTRACTDIALOGCTC_H

#include "popwidget/MoveableDialog.h"

namespace Ui {
class RegisterContractDialogCTC;
}

class RegisterContractDialogCTC : public MoveableDialog
{
    Q_OBJECT

public:
    explicit RegisterContractDialogCTC(QWidget *parent = 0);
    ~RegisterContractDialogCTC();
private slots:
    void jsonDataUpdated(const QString &id,const QString &data);

    void on_okBtn_clicked();

    void on_cancelBtn_clicked();

    void on_closeBtn_clicked();

    void testRegister();
private:
    void InitWidget();
    void InitAccountAddress();

    double parseTestRegisterFee(const QString &data)const;
private:
    Ui::RegisterContractDialogCTC *ui;
};

#endif // REGISTERCONTRACTDIALOGCTC_H
