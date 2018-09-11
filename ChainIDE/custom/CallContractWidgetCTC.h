#ifndef CALLCONTRACTWIDGETCTC_H
#define CALLCONTRACTWIDGETCTC_H

#include "popwidget/MoveableDialog.h"

namespace Ui {
class CallContractWidgetCTC;
}

class CallContractWidgetCTC : public MoveableDialog
{
    Q_OBJECT

public:
    explicit CallContractWidgetCTC(QWidget *parent = 0);
    ~CallContractWidgetCTC();
private slots:
    void jsonDataUpdated(const QString &id,const QString &data);
    void CallContract();
    void contractAddressChanged();
    void functionChanged();

    //测试调用费用
    void testCallContract();
private:
    void InitWidget();
    void InitAccountAddress();
    void InitContractAddress();

    double parseTestCallFee(const QString &data)const;
private:
    Ui::CallContractWidgetCTC *ui;
};

#endif // CALLCONTRACTWIDGETCTC_H
