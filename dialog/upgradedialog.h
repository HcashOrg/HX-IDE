#ifndef UPGRADEDIALOG_H
#define UPGRADEDIALOG_H

#include <QDialog>

namespace Ui {
class UpgradeDialog;
}

class UpgradeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UpgradeDialog(QString contractAddress, QWidget *parent = 0);
    ~UpgradeDialog();

protected:
    void mousePressEvent(QMouseEvent*event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *);

private slots:

    void jsonDataUpdated(QString id);

    void on_okBtn_clicked();

    void on_cancelBtn_clicked();

    void on_closeBtn_clicked();

    void on_estimateBtn_clicked();

    void on_onUpgradeAmountLineEdit_textChanged(const QString &arg1);

private:
    Ui::UpgradeDialog *ui;
    bool mouse_press;
    QPoint move_point;
    QString address;

    bool costCalculated;     // 计算预计费用后置为true

    void validateOkBtn();
    void paintEvent(QPaintEvent*);

};

#endif // UPGRADEDIALOG_H
