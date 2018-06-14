#ifndef TRANSFERTOACCOUNTDIALOG_H
#define TRANSFERTOACCOUNTDIALOG_H

#include <QDialog>

namespace Ui {
class TransferToAccountDialog;
}

class TransferToAccountDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TransferToAccountDialog(QWidget *parent = 0);
    ~TransferToAccountDialog();

protected:
    void mousePressEvent(QMouseEvent*event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *);

public slots:
    void updateAccountBalance();

private slots:
    void jsonDataUpdated(QString id);

    void on_accountComboBox_currentIndexChanged(const QString &arg1);

    void on_okBtn_clicked();

    void on_sendToLineEdit_textChanged(const QString &arg1);

    void on_amountLineEdit_textChanged(const QString &arg1);

    void on_closeBtn_clicked();

    void on_remarkLineEdit_textChanged(const QString &arg1);

    void on_cancelBtn_clicked();

private:
    Ui::TransferToAccountDialog *ui;
    bool inited;
    bool mouse_press;
    QPoint move_point;

    void paintEvent(QPaintEvent*);
    void validateOkBtn();
};

#endif // TRANSFERTOACCOUNTDIALOG_H
