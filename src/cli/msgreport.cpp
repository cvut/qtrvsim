#include "msgreport.h"

#include <iostream>

using namespace std;

MsgReport::MsgReport(QCoreApplication *app) : Super(app) {}

void MsgReport::report_message(
    messagetype::Type type,
    const QString &file,
    int line,
    int column,
    const QString &text,
    const QString &hint) {
    (void)hint;

    QString typestr = "error";
    switch (type) {
    case messagetype::MSG_ERROR: typestr = "error"; break;
    case messagetype::MSG_WARNING: typestr = "warning"; break;
    case messagetype::MSG_INFO: typestr = "info"; break;
    default: return;
    }

    printf("%s:", qPrintable(file));
    if (line != 0) { printf("%d:", line); }
    if (column != 0) { printf("%d:", column); }
    printf("%s:%s\n", qPrintable(typestr), qPrintable(text));
}
