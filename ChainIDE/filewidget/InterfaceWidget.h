#ifndef INTERFACEWIDGET_H
#define INTERFACEWIDGET_H

#include <QWidget>
#include "DataDefine.h"

namespace Ui {
class InterfaceWidget;
}

class InterfaceWidget : public QWidget
{
    Q_OBJECT

public:
    explicit InterfaceWidget(QWidget *parent = 0);
    ~InterfaceWidget();
public slots:
    void ShowInterface(const QString &filePath);
    void retranslator();
private:
    bool readApiFromPath(const QString &filePath, DataDefine::ApiEventPtr &results);
private:
    void InitData();
private:
    void InitWidget();
private:
    Ui::InterfaceWidget *ui;
    class DataPrivate;
    DataPrivate *_p;
};

#endif // INTERFACEWIDGET_H
