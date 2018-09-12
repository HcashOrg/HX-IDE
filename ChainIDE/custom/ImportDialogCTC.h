#ifndef IMPORTDIALOGCTC_H
#define IMPORTDIALOGCTC_H

#include "popwidget/MoveableDialog.h"

namespace Ui {
class ImportDialogCTC;
}

class ImportDialogCTC : public MoveableDialog
{
    Q_OBJECT

public:
    explicit ImportDialogCTC(QWidget *parent = 0);
    ~ImportDialogCTC();

private slots:
    void on_okBtn_clicked();

    void on_cancelBtn_clicked();

    void on_closeBtn_clicked();

    void jsonDataUpdated(const QString &id,const QString &data);

    void on_pathBtn_clicked();
private:
    Ui::ImportDialogCTC *ui;
};

#endif // IMPORTDIALOGCTC_H
