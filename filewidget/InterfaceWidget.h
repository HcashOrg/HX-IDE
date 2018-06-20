#ifndef INTERFACEWIDGET_H
#define INTERFACEWIDGET_H

#include <QWidget>

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
