#ifndef AboutWidget_H
#define AboutWidget_H

#include "MoveableDialog.h"

namespace Ui {
class AboutWidget;
}

class AboutWidget : public MoveableDialog
{
    Q_OBJECT

public:
    explicit AboutWidget(QWidget *parent = 0);
    ~AboutWidget();
signals:
    void RestartSignal();//更新并重启
    void UpdateNeeded(bool);//是否需要更新
private slots:
    void CheckUpdateSlot();
    void CheckResultSlot(const QString &version);

    void UpdateSlot();

    void UpdateFinishSlot();
    void UpdateWrongSlot();

private:
    void InitWidget();

    void showButtonState(int type);//0--检查升级  1--升级 2--重启
private:
    Ui::AboutWidget *ui;
    class DataPrivate;
    DataPrivate *_p;
};

#endif // AboutWidget_H
