#ifndef REMOVESCRIPTDIALOG_H
#define REMOVESCRIPTDIALOG_H

#include <QDialog>

namespace Ui {
class RemoveScriptDialog;
}

class RemoveScriptDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RemoveScriptDialog( QString scriptId, QWidget *parent = 0);
    ~RemoveScriptDialog();

signals:
    void scriptRemoved(QString address);

protected:
    void mousePressEvent(QMouseEvent*event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *);

private slots:
    void on_okBtn_clicked();

    void jsonDataUpdated(QString id);

    void on_pwdLineEdit_returnPressed();


    void on_closeBtn_clicked();

    void on_cancelBtn_clicked();

private:
    Ui::RemoveScriptDialog *ui;
    bool mouse_press;
    QPoint move_point;

    void paintEvent(QPaintEvent*);
};

#endif // REMOVESCRIPTDIALOG_H
