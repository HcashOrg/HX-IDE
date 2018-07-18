#ifndef ImportDialogHXHX_H
#define ImportDialogHXHX_H

#include "popwidget/MoveableDialog.h"

namespace Ui {
class ImportDialogHX;
}

class ShadowWidget;

class ImportDialogHX : public MoveableDialog
{
    Q_OBJECT

public:
    explicit ImportDialogHX(QWidget *parent = 0);
    ~ImportDialogHX();

private slots:
    void on_okBtn_clicked();

    void on_cancelBtn_clicked();

    void on_closeBtn_clicked();

    void jsonDataUpdated(const QString &id,const QString &data);

    void on_pathBtn_clicked();

private:
    Ui::ImportDialogHX *ui;
};

#endif // ImportDialogHXHX_H
