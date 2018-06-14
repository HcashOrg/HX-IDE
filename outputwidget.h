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

    void appendText(int widgetNum, QString text);   // widgetNum: 0编译输出窗口  1output窗口

    void changeToTest();

    void changeToFormal();
signals:
    void outputRead(QString);

public slots:
    void testHasOutputToRead();

    void formalHasOutputToRead();

private slots:
    void jsonDataUpdated(QString id);

    void on_compileBtn_clicked();

    void on_outputBtn_clicked();

private:
    Ui::OutputWidget *ui;

    QVBoxLayout* vbLayout2;
    QVBoxLayout* vbLayout3;
};

#endif // OUTPUTWIDGET_H
