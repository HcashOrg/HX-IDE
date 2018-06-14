#ifndef ADDSCRIPTDIALOG_H
#define ADDSCRIPTDIALOG_H

#include <QDialog>

namespace Ui {
class AddScriptDialog;
}

class AddScriptDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddScriptDialog( QString path, QWidget *parent = 0);
    ~AddScriptDialog();

protected:
    void mousePressEvent(QMouseEvent*event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *);

private slots:
    void on_okBtn_clicked();

    void jsonDataUpdated(QString id);

    void on_cancelBtn_clicked();

    void on_closeBtn_clicked();

signals:
    void scriptAdded(QString path);

private:
    Ui::AddScriptDialog *ui;
    bool mouse_press;
    QPoint move_point;
    QString scriptPath;

    void paintEvent(QPaintEvent*);
};

#endif // ADDSCRIPTDIALOG_H
