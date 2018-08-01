#ifndef STATUSBAR_H
#define STATUSBAR_H

#include <QStatusBar>

class StatusBar : public QStatusBar
{
    Q_OBJECT
public:
    explicit StatusBar(QWidget *parent = nullptr);
    ~StatusBar();
public:
    void startStatus();
public:
    void setPermanentMessage(const QString &mes);
    void setNormalMessage(const QString &mes);
    void setTempMessage(const QString &mes);
private:
    void queryBlock();
private slots:
    void jsonDataUpdated(const QString &id,const QString &data);
private:
    void InitWidget();
private:
    class DataPrivate;
    DataPrivate *_p;
};

#endif // STATUSBAR_H
