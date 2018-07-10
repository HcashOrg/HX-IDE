#ifndef PASSWORDVERIFYWIDGET_H
#define PASSWORDVERIFYWIDGET_H

#include "MoveableDialog.h"

namespace Ui {
class PasswordVerifyWidget;
}

class PasswordVerifyWidget : public MoveableDialog
{
    Q_OBJECT

public:
    explicit PasswordVerifyWidget(QWidget *parent = 0);
    ~PasswordVerifyWidget();
public:
    bool pop();
private slots:
    void ConfirmSlots();

    void passwordChangeSlots(const QString &address);

    void jsonDataUpdated(const QString &id,const QString &data);

    void passwordReturnPressed();
private:
    void InitWidget();
private:
    Ui::PasswordVerifyWidget *ui;
    bool confirm;
};

#endif // PASSWORDVERIFYWIDGET_H
