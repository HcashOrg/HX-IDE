#ifndef NEWFILEDIALOG_H
#define NEWFILEDIALOG_H

#include <QDialog>
#include <QMouseEvent>

namespace Ui {
class NewFileDialog;
}

class NewFileDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewFileDialog(const QString &topDir,const QStringList &docs,QWidget *parent = 0);
    ~NewFileDialog();
public:
    const QString &getNewFilePath()const;
private slots:
    void TextChanged(const QString &text);

    void comboBoxTextChanged(const QString &text);
    void ConfirmSlot();
    void CancelSlot();
private:
    void InitWidget();
    void ValidFile();
protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent*event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *);
private:
    Ui::NewFileDialog *ui;
    class DataPrivate;
    DataPrivate *_p;
    bool mouse_press;
    QPoint move_point;
};

#endif // NEWFILEDIALOG_H
