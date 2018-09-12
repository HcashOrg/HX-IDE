#ifndef NAMEDIALOGCTC_H
#define NAMEDIALOGCTC_H

#include "popwidget/MoveableDialog.h"

namespace Ui {
class NameDialogCTC;
}

class NameDialogCTC : public MoveableDialog
{
    Q_OBJECT

public:
    explicit NameDialogCTC(QWidget *parent = 0);
    ~NameDialogCTC();

    QString pop();

private slots:
    void on_okBtn_clicked();

    void on_cancelBtn_clicked();

    void on_nameLineEdit_textChanged(const QString &arg1);

    void on_nameLineEdit_returnPressed();

    void on_closeBtn_clicked();
private:
    Ui::NameDialogCTC *ui;
    bool yesOrNO;
};

#endif // NAMEDIALOGCTC_H
