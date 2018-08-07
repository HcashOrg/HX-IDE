#ifndef UPDATEWIDGET_H
#define UPDATEWIDGET_H

#include "MoveableDialog.h"

namespace Ui {
class UpdateWidget;
}

class UpdateWidget : public MoveableDialog
{
    Q_OBJECT

public:
    explicit UpdateWidget(const QString &packageName = "update.zip",const QString &mainName = "ChainIDE.exe",const QString &unpackageName = "update",QWidget *parent = 0);
    ~UpdateWidget();
private slots:
    void startMove();

    void copyFinish();

    void restartWallet();
private:
    void InitWidget();
    void InitStyle();
private:
    Ui::UpdateWidget *ui;
    class DataPrivate;
    DataPrivate *_p;
};

#endif // UPDATEWIDGET_H
