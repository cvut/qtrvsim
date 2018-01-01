#include "qtmipsexception.h"
#include <iostream>
#include <cstring>

using namespace machine;

QtMipsException::QtMipsException(QString reason, QString ext, QString file, int line) {
    this->name = "Exception";
    this->reason = reason;
    this->ext = ext;
    this->file = file;
    this->line = line;
}

const char *QtMipsException::what() const throw() {
    QString message = this->msg(true);
    char * cstr = new char [message.length()+1];
    std::strcpy (cstr, message.toStdString().c_str());
    return cstr;
}

QString QtMipsException::msg(bool pos) const {
    QString message;
    message += name;
    if (pos)
        message += QString(" (") + QString(this->file) + QString(":") + QString::number(this->line) + QString(")");
    message += ": " + this->reason;
    if (!this->ext.isEmpty()) {
        message += QString(": ");
        message += this->ext;
    }
    return message;
}

#define EXCEPTION(NAME, PARENT) \
    QtMipsException##NAME::QtMipsException##NAME(QString reason, QString ext, QString file, int line) \
        : QtMipsException##PARENT(reason, ext, file, line) { \
        name = #NAME; \
    }
QTMIPS_EXCEPTIONS
#undef EXCEPTION
