#ifndef CallContractWidgetUB_H
#define CallContractWidgetUB_H

#include "popwidget/MoveableDialog.h"

namespace Ui {
class CallContractWidgetUB;
}

class CallContractWidgetUB : public MoveableDialog
{
    Q_OBJECT

public:
    explicit CallContractWidgetUB(QWidget *parent = 0);
    ~CallContractWidgetUB();
private slots:
    void jsonDataUpdated(const QString &id,const QString &data);
    void CallContract();
    void contractAddressChanged();
private:
    void InitWidget();
    void InitAccountAddress();
    void InitContractAddress();
private:
    Ui::CallContractWidgetUB *ui;
private:

    bool parseContractInfo(const QString &addr,const QString &data);
};

#endif // CallContractWidgetUB_H
