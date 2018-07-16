#ifndef NameDialogHX_H
#define NameDialogHX_H

#include "popwidget/MoveableDialog.h"

namespace Ui {
class NameDialogHX;
}

class NameDialogHX : public MoveableDialog
{
    Q_OBJECT

public:
    explicit NameDialogHX(QWidget *parent = 0);
    ~NameDialogHX();

    QString pop();

private slots:
    void on_okBtn_clicked();

    void on_cancelBtn_clicked();

    void on_nameLineEdit_textChanged(const QString &arg1);

    void on_nameLineEdit_returnPressed();

    void on_closeBtn_clicked();

private:
    Ui::NameDialogHX *ui;
    bool yesOrNO;
};

#endif // NameDialogHX_H
