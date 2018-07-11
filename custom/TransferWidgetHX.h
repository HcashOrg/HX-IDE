#ifndef TransferWidgetHX_H
#define TransferWidgetHX_H

#include "popwidget/MoveableDialog.h"

namespace Ui {
class TransferWidgetHX;
}

class TransferWidgetHX : public MoveableDialog
{
    Q_OBJECT

public:
    explicit TransferWidgetHX(QWidget *parent = 0);
    ~TransferWidgetHX();
private slots:
    void transferSlot();
    void comboBoxAccountChangeSlot();
    void comboBoxAssetChangeSlot();
    void jsonDataUpdated(const QString &id,const QString &data);
private:
    void InitWidget();
    void InitComboBox();
private:
    Ui::TransferWidgetHX *ui;
};

#endif // TransferWidgetHX_H
