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
private:
    void InitWidget();
    void InitAccountAddress();
    void InitContractAddress();
private:
    Ui::CallContractWidgetHX *ui;
private:

    bool parseContractInfo(const QString &addr,const QString &data);
};

#endif // CALLCONTRACTWIDGETHX_H
