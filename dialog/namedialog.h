#ifndef NAMEDIALOG_H
#define NAMEDIALOG_H

#include <QDialog>

namespace Ui {
class NameDialog;
}

class NameDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NameDialog(QWidget *parent = 0);
    ~NameDialog();

    QString pop();

protected:
    void mousePressEvent(QMouseEvent*event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *);

private slots:
    void on_okBtn_clicked();

    void on_cancelBtn_clicked();

    void on_nameLineEdit_textChanged(const QString &arg1);

    void on_nameLineEdit_returnPressed();

    void jsonDataUpdated(QString id);

    void on_closeBtn_clicked();

private:
    Ui::NameDialog *ui;
    bool mouse_press;
    QPoint move_point;
    bool yesOrNO;
    QMovie* gif;

    void paintEvent(QPaintEvent*);
};

#endif // NAMEDIALOG_H
