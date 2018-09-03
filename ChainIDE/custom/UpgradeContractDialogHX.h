#ifndef UPGRADECONTRACTDIALOGHX_H
#define UPGRADECONTRACTDIALOGHX_H

#include "popwidget/MoveableDialog.h"

namespace Ui {
class UpgradeContractDialogHX;
}

class UpgradeContractDialogHX : public MoveableDialog
{
    Q_OBJECT

public:
    explicit UpgradeContractDialogHX(QWidget *parent = 0);
    ~UpgradeContractDialogHX();
private slots:
    void jsonDataUpdated(const QString &id,const QString &data);
    void UpgradeContract();
private:
    void InitWidget();
    void InitAccountAddress();
    void refreshContractAddress();
private:
    Ui::UpgradeContractDialogHX *ui;
};

#endif // UPGRADECONTRACTDIALOGHX_H
