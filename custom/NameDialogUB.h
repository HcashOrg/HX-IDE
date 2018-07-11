#ifndef NameDialogUB_H
#define NameDialogUB_H

#include <QDialog>
#include "popwidget/MoveableDialog.h"

namespace Ui {
class NameDialogUB;
}

class NameDialogUB : public MoveableDialog
{
    Q_OBJECT

public:
    explicit NameDialogUB(QWidget *parent = 0);
    ~NameDialogUB();

    QString pop();

private slots:
    void on_okBtn_clicked();

    void on_cancelBtn_clicked();

    void on_nameLineEdit_textChanged(const QString &arg1);

    void on_nameLineEdit_returnPressed();

    void on_closeBtn_clicked();

private:
    Ui::NameDialogUB *ui;
    bool yesOrNO;
};

#endif // NameDialogUB_H
