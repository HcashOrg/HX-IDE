#ifndef BRIDGE_H
#define BRIDGE_H

#include <QObject>
#include <QJSValue>
#include <functional>
#include <QVariant>

class AceEditor;

class bridge : QObject
{
Q_OBJECT
public:
    static bridge* instance();
    void setEditor(AceEditor *e);

signals:
    void textChanged();
    void markChanged(int ,bool);
    void gutterRightClickedSignal();
public slots:
    void onTextChanged();

    void onMarkChange(int linenumber,bool isadd);

    void gutterRightClicked();
private:
    bridge();

private:
    AceEditor *editor;
};

#endif // BRIDGE_H
