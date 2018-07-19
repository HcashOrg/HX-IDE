#ifndef DEBUGWIDGET_H
#define DEBUGWIDGET_H

#include <QWidget>

namespace Ui {
class DebugWidget;
}
//调试窗口
class DebugWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DebugWidget(QWidget *parent = 0);
    ~DebugWidget();
private:
    void InitWidget();
private:
    Ui::DebugWidget *ui;
    class DataPrivate;
    DataPrivate *_p;
};

#endif // DEBUGWIDGET_H
