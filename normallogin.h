#ifndef NORMALLOGIN_H
#define NORMALLOGIN_H

#include <QWidget>
#include <QFileInfo>
#include <QTimer>
#include <QLibrary>

namespace Ui {
class NormalLogin;
}

typedef short(*FUN)(int);

class NormalLogin : public QWidget
{
    Q_OBJECT

public:
    explicit NormalLogin( int chainType = 1, QWidget *parent = 0);
    ~NormalLogin();

private slots:

    void on_enterBtn_clicked();

    void on_pwdLineEdit_returnPressed();

    void on_pwdLineEdit_textChanged(const QString &arg1);

    void pwdConfirmed(QString id);

    int  lockTime();

    void checkLock();

signals:
    void login();
    void minimum();
    void closeIDE();
    void tray();
    void showShadowWidget();
    void hideShadowWidget();

private:
    Ui::NormalLogin *ui;

    int chances;
    QTimer* timerForLockTime;
    int type;

};

#endif // NORMALLOGIN_H
