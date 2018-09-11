#ifndef CALLCONTRACTWIDGETHX_H
#define CALLCONTRACTWIDGETHX_H

#include "popwidget/MoveableDialog.h"

namespace Ui {
class CallContractWidgetHX;
}

class CallContractWidgetHX : public MoveableDialog
{
    Q_OBJECT

public:
    explicit CallContractWidgetHX(QWidget *parent = 0);
    ~CallContractWidgetHX();
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
    Ui::CallContractWidgetHX *ui;
};

#endif // CALLCONTRACTWIDGETHX_H
