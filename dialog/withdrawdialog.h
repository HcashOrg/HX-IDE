#ifndef WITHDRAWDIALOG_H
#define WITHDRAWDIALOG_H

#include <QDialog>

namespace Ui {
class WithdrawDialog;
}

class WithdrawDialog : public QDialog
{
    Q_OBJECT

public:
    explicit WithdrawDialog( QString contractAddress, QWidget *parent = 0);
    ~WithdrawDialog();

signals:
    void contractDestroyed(QString address);

protected:
    void mousePressEvent(QMouseEvent*event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *);

private slots:
    void on_okBtn_clicked();

    void jsonDataUpdated(QString id);

    void on_pwdLineEdit_returnPressed();

    void on_closeBtn_clicked();

    void on_estimateBtn_clicked();

    void on_onDestroyAmountLineEdit_textChanged(const QString &arg1);

    void on_cancelBtn_clicked();

private:
    Ui::WithdrawDialog *ui;
    bool mouse_press;
    QPoint move_point;
    QString address;
    QString ownerName;

    bool costCalculated;     // 计算预计费用后置为true

    void validateOkBtn();
    void paintEvent(QPaintEvent*);
};

#endif // WITHDRAWDIALOG_H
