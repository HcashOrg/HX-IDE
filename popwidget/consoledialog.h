#ifndef CONSOLEDIALOG_H
#define CONSOLEDIALOG_H

#include <QDialog>
#include "MoveableDialog.h"

namespace Ui {
class ConsoleDialog;
}

class ConsoleDialog : public MoveableDialog
{
    Q_OBJECT

public:
    explicit ConsoleDialog(QWidget *parent = 0);
    ~ConsoleDialog();

    void pop();

private slots:
    void on_closeBtn_clicked();

    void on_consoleLineEdit_returnPressed();

    void jsonDataUpdated(const QString &id,const QString &data);

    void on_clearBtn_clicked();

private:
    Ui::ConsoleDialog *ui;
    QVector<QString> cmdVector;
    int cmdIndex;

    bool eventFilter(QObject *watched, QEvent *e);
};

#endif // CONSOLEDIALOG_H
