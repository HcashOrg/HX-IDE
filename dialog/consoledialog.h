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

public slots:
    void outputRead(QString output);

protected:
    void mousePressEvent(QMouseEvent*event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *);

private slots:

    void on_consoleLineEdit_returnPressed();

    void jsonDataUpdated(QString id);

    void on_closeBtn_clicked();

private:
    Ui::ConsoleDialog *ui;
    bool mouse_press;
    QPoint move_point;

    void paintEvent(QPaintEvent*);
};

#endif // CONSOLEDIALOG_H
