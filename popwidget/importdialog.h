#ifndef IMPORTDIALOG_H
#define IMPORTDIALOG_H

#include "MoveableDialog.h"

namespace Ui {
class ImportDialog;
}

class ShadowWidget;

class ImportDialog : public MoveableDialog
{
    Q_OBJECT

public:
    explicit ImportDialog(QWidget *parent = 0);
    ~ImportDialog();

private slots:
    void on_okBtn_clicked();

    void on_cancelBtn_clicked();

    void on_privatekey_returnPressed();

    void on_closeBtn_clicked();

    void jsonDataUpdated(const QString &id,const QString &data);

private:
    Ui::ImportDialog *ui;
};

#endif // IMPORTDIALOG_H
