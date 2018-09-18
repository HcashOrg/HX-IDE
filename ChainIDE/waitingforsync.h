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
    void closeIDE();
public slots:
    void ReceiveMessage(const QString &message);
private:
    void InitWidget();
private:
    Ui::WaitingForSync *ui;

};

#endif // WAITINGFORSYNC_H
