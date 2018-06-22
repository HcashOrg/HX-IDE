#ifndef CONSOLEDIALOG_H
#define CONSOLEDIALOG_H

#include <QDialog>

namespace Ui {
class ConsoleDialog;
}

class ConsoleDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConsoleDialog(QWidget *parent = 0);
    ~ConsoleDialog();

    void pop();

protected:
    void mousePressEvent(QMouseEvent*event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *);
    void paintEvent(QPaintEvent *);

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
    bool mouse_press;
    QPoint move_point;
};

#endif // CONSOLEDIALOG_H
