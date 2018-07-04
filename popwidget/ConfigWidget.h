#ifndef CONFIGWIDGET_H
#define CONFIGWIDGET_H

#include "MoveableDialog.h"

namespace Ui {
class ConfigWidget;
}

class ConfigWidget : public MoveableDialog
{
    Q_OBJECT

public:
    explicit ConfigWidget(QWidget *parent = 0);
    ~ConfigWidget();
public:
    bool pop();
private slots:
    void ConfirmSlots();
private:
    void InitWidget();
private:
    Ui::ConfigWidget *ui;
    bool isOk;
};

#endif // CONFIGWIDGET_H
