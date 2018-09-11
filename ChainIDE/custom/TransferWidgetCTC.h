#ifndef TRANSFERWIDGETCTC_H
#define TRANSFERWIDGETCTC_H

#include "popwidget/MoveableDialog.h"

namespace Ui {
class TransferWidgetCTC;
}

class TransferWidgetCTC : public MoveableDialog
{
    Q_OBJECT

public:
    explicit TransferWidgetCTC(QWidget *parent = 0);
    ~TransferWidgetCTC();
private slots:
    void transferSlot();
    void comboBoxAccountChangeSlot();
    void comboBoxAssetChangeSlot();
    void jsonDataUpdated(const QString &id,const QString &data);
private:
    void InitWidget();
    void InitComboBox();
private:
    Ui::TransferWidgetCTC *ui;
};

#endif // TRANSFERWIDGETCTC_H
