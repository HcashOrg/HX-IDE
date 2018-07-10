#ifndef NAMEDIALOG_H
#define NAMEDIALOG_H

#include "MoveableDialog.h"

namespace Ui {
class NameDialog;
}

class NameDialog : public MoveableDialog
{
    Q_OBJECT

public:
    explicit NameDialog(QWidget *parent = 0);
    ~NameDialog();

    QString pop();

private slots:
    void on_okBtn_clicked();

    void on_cancelBtn_clicked();

    void on_nameLineEdit_textChanged(const QString &arg1);

    void on_nameLineEdit_returnPressed();

    void on_closeBtn_clicked();

private:
    Ui::NameDialog *ui;
    bool yesOrNO;
};

#endif // NAMEDIALOG_H
