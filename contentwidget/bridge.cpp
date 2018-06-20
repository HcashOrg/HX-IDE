#include "bridge.h"
#include "aceeditor.h"
#include <QMessageBox>
#include <QJSEngine>
#include <QDebug>

bridge* bridge::instance()
{
    static bridge s_obj;
    return &s_obj;
}

bridge::bridge()
{
}

void bridge::setEditor(AceEditor *e)
{
    this->editor = e;
}

void bridge::onTextChanged()
{
    emit textChanged();
}
