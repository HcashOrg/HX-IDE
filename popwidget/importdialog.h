#ifndef IMPORTDIALOG_H
#define IMPORTDIALOG_H

#include <QDialog>

namespace Ui {
class ImportDialog;
}

class ShadowWidget;

class ImportDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ImportDialog(QWidget *parent = 0);
    ~ImportDialog();

signals:
    void accountImported();

protected:
    void mousePressEvent(QMouseEvent*event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *);

private slots:
    void on_pathBtn_clicked();

    void on_okBtn_clicked();

    void jsonDataUpdated(QString id);

    void on_privateKeyLineEdit_textChanged(const QString &arg1);

    void on_privateKeyLineEdit_returnPressed();

    void on_closeBtn_clicked();

    void on_cancelBtn_clicked();

private:
    Ui::ImportDialog *ui;
    bool mouse_press;
    QPoint move_point;

    void paintEvent(QPaintEvent*);
};

#endif // IMPORTDIALOG_H
