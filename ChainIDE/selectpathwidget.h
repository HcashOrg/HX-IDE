#ifndef SELECTPATHWIDGET_H
#define SELECTPATHWIDGET_H

#include "popwidget/MoveableDialog.h"

namespace Ui {
class SelectPathWidget;
}
//选择数据路径界面
class SelectPathWidget : public MoveableDialog
{
    Q_OBJECT

public:
    explicit SelectPathWidget(QWidget *parent = 0);
    ~SelectPathWidget();

private slots:
    void on_pathBtn_clicked();

    void on_okBtn_clicked();

    void on_closeBtn_clicked();
signals:
    void enter();
    void cancel();

private:
    Ui::SelectPathWidget *ui;
};

#endif // SELECTPATHWIDGET_H
