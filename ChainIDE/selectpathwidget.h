#ifndef SELECTPATHWIDGET_H
#define SELECTPATHWIDGET_H

#include <QWidget>
#include <QtNetwork>

#include "popwidget/MoveableDialog.h"
namespace Ui {
class SelectPathWidget;
}

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
