#ifndef ImportDialogUB_H
#define ImportDialogUB_H

#include "popwidget/MoveableDialog.h"

namespace Ui {
class ImportDialogUB;
}

class ShadowWidget;

class ImportDialogUB : public MoveableDialog
{
    Q_OBJECT

public:
    explicit ImportDialogUB(QWidget *parent = 0);
    ~ImportDialogUB();

private slots:
    void on_okBtn_clicked();

    void on_cancelBtn_clicked();

    void on_privatekey_returnPressed();

    void on_closeBtn_clicked();

    void jsonDataUpdated(const QString &id,const QString &data);

private:
    Ui::ImportDialogUB *ui;
};

#endif // ImportDialogUB_H
