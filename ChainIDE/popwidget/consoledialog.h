#ifndef CONSOLEDIALOG_H
#define CONSOLEDIALOG_H

#include <QDialog>
#include <QStringList>
#include "popwidget/MoveableDialog.h"

namespace Ui {
class ConsoleDialog;
}

class ConsoleDialog : public MoveableDialog
{
    Q_OBJECT

public:
    explicit ConsoleDialog(QWidget *parent = 0);
    ~ConsoleDialog();

private slots:
    void on_closeBtn_clicked();

    void on_consoleLineEdit_returnPressed();

    void jsonDataUpdated(const QString &id,const QString &data);

    void on_clearBtn_clicked();
private:
    enum IndexType{Up,Down,Last,First};
    void ModifyIndex(IndexType indexType);//往更早之前的输入进行索引
private:
    Ui::ConsoleDialog *ui;
    QStringList cmds;
    int cmdIndex;

    bool eventFilter(QObject *watched, QEvent *e);
};

#endif // CONSOLEDIALOG_H
