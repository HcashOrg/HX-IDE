#ifndef DEBUGFUNCTIONWIDGET_H
#define DEBUGFUNCTIONWIDGET_H

#include <QDialog>
#include "DataDefine.h"
#include "popwidget/MoveableDialog.h"

namespace Ui {
class DebugFunctionWidget;
}

class DebugFunctionWidget : public MoveableDialog
{
    Q_OBJECT

public:
    explicit DebugFunctionWidget(const QString &file,DataDefine::ApiEventPtr apis,QWidget *parent = 0);
    ~DebugFunctionWidget();
public:
    const QString &SelectedApi()const;
    const QStringList &ApiParams()const;
private slots:
    void OnOKClicked();
private:
    void InitWidget(const QString &file,DataDefine::ApiEventPtr apis);
private:
    Ui::DebugFunctionWidget *ui;
    QString api;
    QStringList params;
};

#endif // DEBUGFUNCTIONWIDGET_H
