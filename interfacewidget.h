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

    void setContract(QString contractAddress);

    void setScript(QString scriptId);

signals:
    void eventBindTriggered(QString contractAddress);

public slots:
    void contractInfoUpdated(QString contractAddress);

private slots:
    void jsonDataUpdated(QString id);

    void on_interfaceBtn_clicked();

    void on_eventBtn_clicked();

    void on_eventTreeWidget_customContextMenuRequested(const QPoint &pos);

    void unbind();

    void onEventBindTriggered();

private:
    Ui::InterfaceWidget *ui;
    QString address;
};

#endif // INTERFACEWIDGET_H
