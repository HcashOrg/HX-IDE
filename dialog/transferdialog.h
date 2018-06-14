#ifndef TRANSFERDIALOG_H
#define TRANSFERDIALOG_H

#include <QDialog>

namespace Ui {
class TransferDialog;
}

class TransferDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TransferDialog( QString contractAddress, QWidget *parent = 0);
    ~TransferDialog();

public slots:
    void updateAccountBalance();

    void contractInfoUpdated(QString addressOrName);

protected:
    void mousePressEvent(QMouseEvent*event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *);

private slots:

    void jsonDataUpdated(QString id);

    void on_okBtn_clicked();

    void on_cancelBtn_clicked();

    void on_estimateBtn_clicked();

    void on_addressOrNameLineEdit_textChanged(const QString &arg1);

    void on_amountLineEdit_textChanged(const QString &arg1);

    void on_closeBtn_clicked();

    void on_onDepositAmountLineEdit_textChanged(const QString &arg1);

private:
    Ui::TransferDialog *ui;
    bool mouse_press;
    QPoint move_point;

    bool costCalculated;     // 合约地址和金额 修改后置为false  计算预计费用后置为true

    void validateOkBtn();
    void paintEvent(QPaintEvent*);
};

#endif // TRANSFERDIALOG_H
