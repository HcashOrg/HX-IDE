#ifndef OUTPUTMESSAGE
#define OUTPUTMESSAGE

#include <qapplication.h>
#include <stdio.h>
#include <stdlib.h>

void outputMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg);

#endif // OUTPUTMESSAGE

