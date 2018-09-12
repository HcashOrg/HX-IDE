#ifndef ImportEnterPwdDialog_H
#define ImportEnterPwdDialog_H

#include "popwidget/MoveableDialog.h"

namespace Ui {
class ImportEnterPwdDialog;
}

class ImportEnterPwdDialog : public MoveableDialog
{
    Q_OBJECT

public:
    explicit ImportEnterPwdDialog(QWidget *parent = 0);
    ~ImportEnterPwdDialog();

    bool  pop();

    QString pwd;

private slots:
    void on_okBtn_clicked();

    void on_cancelBtn_clicked();

    void on_pkPwdLineEdit_textEdited(const QString &arg1);

    void on_closeBtn_clicked();

private:
    Ui::ImportEnterPwdDialog *ui;
    bool yesOrNO;

    void checkOkBtn();
};

#endif // ImportEnterPwdDialog_H
