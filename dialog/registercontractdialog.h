#ifndef REGISTERCONTRACTDIALOG_H
#define REGISTERCONTRACTDIALOG_H

#include <QDialog>

namespace Ui {
class RegisterContractDialog;
}

class RegisterContractDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RegisterContractDialog(QString path, QWidget *parent = 0);
    ~RegisterContractDialog();

protected:
    void mousePressEvent(QMouseEvent*event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *);

private slots:
    void jsonDataUpdated(QString id);

    void on_okBtn_clicked();

    void on_cancelBtn_clicked();

    void on_initCostLineEdit_textChanged(const QString &arg1);

    void on_closeBtn_clicked();

signals:
    void contractRegistered(QString path);

private:
    Ui::RegisterContractDialog *ui;
    bool mouse_press;
    QPoint move_point;
    QString filePath;

    void paintEvent(QPaintEvent*);
};

#endif // REGISTERCONTRACTDIALOG_H
