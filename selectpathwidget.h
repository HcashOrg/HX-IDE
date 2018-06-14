#ifndef SELECTPATHWIDGET_H
#define SELECTPATHWIDGET_H

#include <QWidget>
#include <QtNetwork>

namespace Ui {
class SelectPathWidget;
}

class SelectPathWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SelectPathWidget(QWidget *parent = 0);
    ~SelectPathWidget();

private slots:
    void on_selectPathBtn_clicked();

    void on_okBtn_clicked();

signals:
    void enter();
    void minimum();
    void closeIDE();
    void showShadowWidget();
    void hideShadowWidget();

private:
    Ui::SelectPathWidget *ui;
};

#endif // SELECTPATHWIDGET_H
