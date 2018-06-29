#ifndef FUNCTIONWIDGET_H
#define FUNCTIONWIDGET_H

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

public:
    void RefreshContractAddr(const QString &addr);
private slots:

    void jsonDataUpdated(const QString &id,const QString &data);
private:
    void InitWidget();
    bool parseContractInfo(const QString &addr, const QString &data);
private:
    Ui::FunctionWidget *ui;
};

#endif // FUNCTIONWIDGET_H
