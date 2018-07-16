#ifndef WAITINGFORSYNC_H
#define WAITINGFORSYNC_H

#include <QWidget>

namespace Ui {
class WaitingForSync;
}

class QNetworkReply;

class WaitingForSync : public QWidget
{
    Q_OBJECT

public:
    explicit WaitingForSync(QWidget *parent = 0);
    ~WaitingForSync();

signals:
    void sync();
    void minimum();
    void closeIDE();

private:
    Ui::WaitingForSync *ui;
    QMovie* gif;

};

#endif // WAITINGFORSYNC_H
