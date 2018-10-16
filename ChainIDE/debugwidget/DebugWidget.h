#ifndef DEBUGWIDGET_H
#define DEBUGWIDGET_H

#include <memory>
#include <QWidget>

namespace Ui {
class DebugWidget;
}

class BaseItemData;
typedef std::shared_ptr<BaseItemData> BaseItemDataPtr;

//调试窗口
class DebugWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DebugWidget(QWidget *parent = 0);
    ~DebugWidget();
public:
    void ResetData(BaseItemDataPtr data);
    void ClearData();
private:
    void InitWidget();
private:
    Ui::DebugWidget *ui;
    class DataPrivate;
    DataPrivate *_p;
};

#endif // DEBUGWIDGET_H
