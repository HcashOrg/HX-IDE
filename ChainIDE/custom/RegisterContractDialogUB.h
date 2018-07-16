#ifndef RegisterContractDialogUB_H
#define RegisterContractDialogUB_H

#include <QDialog>
#include "popwidget/MoveableDialog.h"

namespace Ui {
class RegisterContractDialogUB;
}

class RegisterContractDialogUB : public MoveableDialog
{
    Q_OBJECT

public:
    explicit RegisterContractDialogUB(QWidget *parent = 0);
    ~RegisterContractDialogUB();

private slots:
    void jsonDataUpdated(const QString &id,const QString &data);

    void on_okBtn_clicked();

    void on_cancelBtn_clicked();

    void on_closeBtn_clicked();
private:
    void InitWidget();
    void InitAccountAddress();
private:
    Ui::RegisterContractDialogUB *ui;
    QString contractAddress;
};

#endif // RegisterContractDialogUB_H
