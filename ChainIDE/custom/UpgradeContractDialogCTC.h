#ifndef UPGRADECONTRACTDIALOGCTC_H
#define UPGRADECONTRACTDIALOGCTC_H

#include "popwidget/MoveableDialog.h"

namespace Ui {
class UpgradeContractDialogCTC;
}

class UpgradeContractDialogCTC : public MoveableDialog
{
    Q_OBJECT

public:
    explicit UpgradeContractDialogCTC(QWidget *parent = 0);
    ~UpgradeContractDialogCTC();
private slots:
    void jsonDataUpdated(const QString &id,const QString &data);
    void UpgradeContract();

    void testUpgradeContract();
private:
    void InitWidget();
    void InitAccountAddress();
    void refreshContractAddress();

    double parseTestUpgrade(const QString &data)const;

private:
    Ui::UpgradeContractDialogCTC *ui;
};

#endif // UPGRADECONTRACTDIALOGCTC_H
