#ifndef FunctionWidget_H
#define FunctionWidget_H

#include <QWidget>

namespace Ui {
class FunctionWidget;
}

class FunctionWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FunctionWidget(QWidget *parent = 0);
    ~FunctionWidget();
signals:

public:
    void RefreshContractAddr(const QString &addr);
public slots:
    void retranslator();
private:
    void InitWidget();
private:
    Ui::FunctionWidget *ui;
};

#endif // FunctionWidget_H
