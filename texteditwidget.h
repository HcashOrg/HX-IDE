#ifndef TEXTEDITWIDGET_H
#define TEXTEDITWIDGET_H

#include <QWidget>
#include <QPlainTextEdit>

class TextEditWidget : public QPlainTextEdit
{
    Q_OBJECT
public:
    TextEditWidget( QString path, QWidget *parent = 0);

    bool hasSaved;
    QString filePath;

private slots:
    void onTextChanged();
};

#endif // TEXTEDITWIDGET_H
