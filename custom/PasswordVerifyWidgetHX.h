#ifndef PasswordVerifyWidgetHX_H
#define PasswordVerifyWidgetHX_H

#include "popwidget/MoveableDialog.h"

namespace Ui {
class PasswordVerifyWidgetHX;
}

class PasswordVerifyWidgetHX : public MoveableDialog
{
    Q_OBJECT

public:
    explicit PasswordVerifyWidgetHX(QWidget *parent = 0);
    ~PasswordVerifyWidgetHX();
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
    Ui::PasswordVerifyWidgetHX *ui;
    bool confirm;
};

#endif // PasswordVerifyWidgetHX_H
