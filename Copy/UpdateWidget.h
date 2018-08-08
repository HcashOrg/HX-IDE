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
    //压缩包名称、主程序名称、解压后的文件夹名称、临时文件夹名称
    explicit UpdateWidget(const QString &packageName = "update.zip",const QString &mainName = "ChainIDE.exe",
                          const QString &unpackageName = "update",const QString &tempName = "updatetemp",QWidget *parent = 0);
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
