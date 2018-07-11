#ifndef COMMONDIALOG_H
#define COMMONDIALOG_H

#include "popwidget/MoveableDialog.h"

namespace Ui {
class CommonDialog;
}

class CommonDialog : public MoveableDialog
{
    Q_OBJECT

public:
    enum commonDialogType{OkAndCancel, OkOnly, YesOrNo};
    explicit CommonDialog( commonDialogType _type,QWidget *parent = 0);
    ~CommonDialog();

    bool pop();
    void setText(QString text);

private slots:
    void on_okBtn_clicked();

    void on_cancelBtn_clicked();

    void on_closeBtn_clicked();

private:
    Ui::CommonDialog *ui;
    bool yesOrNO;
};

#endif // COMMONDIALOG_H
