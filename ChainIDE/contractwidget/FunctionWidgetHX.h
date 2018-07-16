#ifndef FunctionWidgetHX_H
#define FunctionWidgetHX_H

#include <QWidget>

namespace Ui {
class FunctionWidgetHX;
}

class FunctionWidgetHX : public QWidget
{
    Q_OBJECT

public:
    explicit FunctionWidgetHX(QWidget *parent = 0);
    ~FunctionWidgetHX();
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
    Ui::FunctionWidgetHX *ui;
};

#endif // FunctionWidgetHX_H
