#ifndef WAITINGFORSYNC_H
#define WAITINGFORSYNC_H

#include <QWidget>
#include <popwidget/MoveableDialog.h>

namespace Ui {
class WaitingForSync;
}

class QNetworkReply;

class WaitingForSync : public MoveableDialog
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
