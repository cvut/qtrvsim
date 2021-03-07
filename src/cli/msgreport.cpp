#include "msgreport.h"

#include <iostream>

using namespace std;

MsgReport::MsgReport(QCoreApplication *app) : Super(app) {
}

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

    cout << file.toLocal8Bit().data() << ":";
    if (line != 0) {
        cout << line << ":";
    }
    if (column != 0) {
        cout << column << ":";
    }

    cout << typestr.toLocal8Bit().data() << ":";
    cout << text.toLocal8Bit().data();
    cout << endl;
}
