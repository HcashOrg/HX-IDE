#ifndef ACCOUNTLISTDIALOG_H
#define ACCOUNTLISTDIALOG_H

#include <QDialog>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QApplication>
#include <QTableWidgetItem>

namespace Ui {
class AccountListDialog;
}

class AccountListDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AccountListDialog(QWidget *parent = 0);
    ~AccountListDialog();


signals:
    void newAccount();

    void currentAccountChanged(QString currentAccount);

public slots:
    void updateAccountList();

protected:
    void mousePressEvent(QMouseEvent*event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *);

private slots:
    void on_addAccountBtn_clicked();

    void jsonDataUpdated(QString id);

    void on_closeBtn_clicked();

    void on_accountListTableWidget_cellClicked(int row, int column);

    void on_searchBtn_clicked();

    void on_okBtn_clicked();

    void on_importBtn_clicked();

    void on_cancelBtn_clicked();

    void on_accountLineEdit_textChanged(const QString &arg1);

    void scrollToCurrentAccount();

private:
    Ui::AccountListDialog *ui;
    bool mouse_press;
    QPoint move_point;
    QString currentSelectedAccount;     // 保存列表中选择的当前账户 确认后才真的改变当前账户
    QTableWidgetItem* currentItem;      // 当前账户行的一个item 为了scrollTo

    void paintEvent(QPaintEvent*);
};


#endif // ACCOUNTLISTDIALOG_H
