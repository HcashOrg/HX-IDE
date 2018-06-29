#ifndef CALLCONTRACTWIDGET_H
#define CALLCONTRACTWIDGET_H

#include "MoveableDialog.h"

namespace Ui {
class CallContractWidget;
}

class CallContractWidget : public MoveableDialog
{
    Q_OBJECT

public:
    explicit CallContractWidget(QWidget *parent = 0);
    ~CallContractWidget();
private slots:
    void jsonDataUpdated(const QString &id,const QString &data);
    void CallContract();
    void callAddressChanged();
    void contractAddressChanged();
private:
    void InitWidget();
    void InitAccountAddress();
    void InitContractAddress();
private:
    Ui::CallContractWidget *ui;
private:

    bool parseContractInfo(const QString &addr,const QString &data);
};

#endif // CALLCONTRACTWIDGET_H
