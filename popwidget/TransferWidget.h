#ifndef TRANSFERWIDGET_H
#define TRANSFERWIDGET_H

#include "MoveableDialog.h"

namespace Ui {
class TransferWidget;
}

class TransferWidget : public MoveableDialog
{
    Q_OBJECT

public:
    explicit TransferWidget(QWidget *parent = 0);
    ~TransferWidget();
private slots:
    void transferSlot();
    void checkBoxChangeSlot();
    void comboBoxChangeSlot();
    void addressChangeSlot();
    void addressCheckSlot(bool isvalid);
    void jsonDataUpdated(const QString &id,const QString &data);
private:
    void InitWidget();
    void InitComboBox();
private:
    Ui::TransferWidget *ui;
};

#endif // TRANSFERWIDGET_H
