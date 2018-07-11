#ifndef RegisterContractDialogHX_H
#define RegisterContractDialogHX_H

#include <QDialog>
#include "popwidget/MoveableDialog.h"

namespace Ui {
class RegisterContractDialogHX;
}

class RegisterContractDialogHX : public MoveableDialog
{
    Q_OBJECT

public:
    explicit RegisterContractDialogHX(QWidget *parent = 0);
    ~RegisterContractDialogHX();

private slots:
    void jsonDataUpdated(const QString &id,const QString &data);

    void on_okBtn_clicked();

    void on_cancelBtn_clicked();

    void on_closeBtn_clicked();
private:
    void InitWidget();
    void InitAccountAddress();
private:
    Ui::RegisterContractDialogHX *ui;
    QString contractAddress;
};

#endif // RegisterContractDialogHX_H
