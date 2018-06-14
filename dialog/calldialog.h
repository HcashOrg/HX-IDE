#ifndef CALLDIALOG_H
#define CALLDIALOG_H

#include <QDialog>

namespace Ui {
class CallDialog;
}

class CallDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CallDialog(QString contractAddressOrName, QWidget *parent = 0);
    ~CallDialog();

public slots:
    void contractInfoUpdated(QString contract);

private slots:
    void on_estimateBtn_clicked();

    void jsonDataUpdated(QString id);

    void on_okBtn_clicked();

    void on_parameterLineEdit_textChanged(const QString &arg1);

    void on_maxCostLineEdit_textChanged(const QString &arg1);

    void on_functionComboBox_currentIndexChanged(const QString &arg1);

    void on_contractLineEdit_textChanged(const QString &arg1);

    void on_closeBtn_clicked();

    void on_cancelBtn_clicked();

protected:
    void mousePressEvent(QMouseEvent*event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *);

private:
    Ui::CallDialog *ui;
    bool mouse_press;
    QPoint move_point;
    bool costCalculated;     // 调用函数和参数 修改后置为false  计算预计费用后置为true

    void validateOkBtn();
    void paintEvent(QPaintEvent*);
};

#endif // CALLDIALOG_H
