#ifndef CONTEXTMENU_H
#define CONTEXTMENU_H

#include <QWidget>
#include <QMenu>

class ContextMenu : public QMenu
{       
    Q_OBJECT
public:
    enum MenuType{ ContractItemType = 1, ContractLuaType, ContractCompiledType, MyTempContractPathType, MyTempContractAddressType, MyForeverContractPathType, MyForeverContractAddressType, ForeverContractType,
                   EventBindScriptType, BindedScriptType, ScriptItemType, ScriptLuaType, ScriptCompiledType, AddedScriptType};

    ContextMenu( MenuType type, QWidget * parent = 0);
    ~ContextMenu();

signals:
    void newFileTriggered();
    void copyTriggered();
    void pasteTriggered();
    void deleteTriggered();
    void importTriggered();
    void exportTriggered();
    void compileTriggered();
    void registerContractTriggered();
    void callTriggered();
    void upgradeTriggered();
    void withdrawTriggered();
    void transferTriggered();
    void addScriptTriggered();
    void removeTriggered();
    void bindTriggered();
    void eventBindTriggered();
    void unbindTriggered();

    void undoTriggered();
    void redoTriggered();
    void cutTriggered();
    void selectAllTriggered();
private:
    MenuType type;

    void createActions();
};

#endif // CONTEXTMENU_H
