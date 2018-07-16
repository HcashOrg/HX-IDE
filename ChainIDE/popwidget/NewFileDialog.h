#ifndef NEWFILEDIALOG_H
#define NEWFILEDIALOG_H

#include <QDialog>
#include <QMouseEvent>
#include "popwidget/MoveableDialog.h"

namespace Ui {
class NewFileDialog;
}

class NewFileDialog : public MoveableDialog
{
    Q_OBJECT

public:
    explicit NewFileDialog(const QString &topDir,const QStringList &docs,const QString &dirpath = "",QWidget *parent = 0);
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
private:
    Ui::NewFileDialog *ui;
    class DataPrivate;
    DataPrivate *_p;
};

#endif // NEWFILEDIALOG_H
