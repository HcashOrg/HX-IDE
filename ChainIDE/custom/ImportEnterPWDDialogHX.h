#ifndef ImportEnterPwdDialogHX_H
#define ImportEnterPwdDialogHX_H

#include "popwidget/MoveableDialog.h"

namespace Ui {
class ImportEnterPwdDialogHX;
}

class ImportEnterPwdDialogHX : public MoveableDialog
{
    Q_OBJECT

public:
    explicit ImportEnterPwdDialogHX(QWidget *parent = 0);
    ~ImportEnterPwdDialogHX();

    bool  pop();

    QString pwd;

private slots:
    void on_okBtn_clicked();

    void on_cancelBtn_clicked();

    void on_pkPwdLineEdit_textEdited(const QString &arg1);

    void on_closeBtn_clicked();

private:
    Ui::ImportEnterPwdDialogHX *ui;
    bool yesOrNO;

    void checkOkBtn();
};

#endif // ImportEnterPwdDialogHX_H
