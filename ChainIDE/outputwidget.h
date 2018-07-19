#ifndef OUTPUTWIDGET_H
#define OUTPUTWIDGET_H

#include <QWidget>
#include <QLayout>

namespace Ui {
class OutputWidget;
}

class OutputWidget : public QWidget
{
    Q_OBJECT

public:
    explicit OutputWidget(QWidget *parent = 0);
    ~OutputWidget();


public slots:

    void receiveCompileMessage(const QString &text);
    void receiveOutputMessage(const QString &text);


    void retranslator();
private:
    Ui::OutputWidget *ui;
    class DataPrivate;
    DataPrivate *_p;
};

#endif // OUTPUTWIDGET_H
