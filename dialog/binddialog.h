#ifndef BINDDIALOG_H
#define BINDDIALOG_H

#include <QDialog>

namespace Ui {
class BindDialog;
}

class BindDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BindDialog( QString id, QString contractAddress, QString event, QWidget *parent = 0);
    ~BindDialog();

protected:
    void mousePressEvent(QMouseEvent*event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *);

private slots:
    void jsonDataUpdated(QString id);

    void on_okBtn_clicked();

    void on_contractAddressLineEdit_textChanged(const QString &arg1);

    void on_closeBtn_clicked();

    void on_bindBtn_2_clicked();

private:
    Ui::BindDialog *ui;
    bool mouse_press;
    QPoint move_point;
    QString scriptId;

    void paintEvent(QPaintEvent*);
};

#endif // BINDDIALOG_H
