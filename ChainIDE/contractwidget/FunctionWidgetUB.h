#ifndef FunctionWidgetUB_H
#define FunctionWidgetUB_H

#include <QWidget>

namespace Ui {
class FunctionWidgetUB;
}

class FunctionWidgetUB : public QWidget
{
    Q_OBJECT

public:
    explicit FunctionWidgetUB(QWidget *parent = 0);
    ~FunctionWidgetUB();
signals:

public:
    void RefreshContractAddr(const QString &addr);
public slots:
    void retranslator();
private slots:

    void jsonDataUpdated(const QString &id,const QString &data);
private:
    void InitWidget();
    bool parseContractInfo(const QString &addr, const QString &data);
private:
    Ui::FunctionWidgetUB *ui;
};

#endif // FunctionWidgetUB_H
