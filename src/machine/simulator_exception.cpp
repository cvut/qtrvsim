#include "simulator_exception.h"

#include <cstring>
#include <iostream>
#include <utility>

using namespace machine;

SimulatorException::SimulatorException(
    QString reason,
    QString ext,
    QString file,
    int line) {
    this->name = "Exception";
    this->reason = std::move(reason);
    this->ext = std::move(ext);
    this->file = std::move(file);
    this->line = line;
}

const char *SimulatorException::what() const noexcept {
    QString message = this->msg(true);
    char *cstr = new char[message.length() + 1];
    std::strcpy(cstr, message.toStdString().c_str());
    return cstr;
}

QString SimulatorException::msg(bool pos) const {
    QString message;
    message += name;
    if (pos) {
        message += QString(" (") + QString(this->file) + QString(":")
                   + QString::number(this->line) + QString(")");
    }
    message += ": " + this->reason;
    if (!this->ext.isEmpty()) {
        message += QString(": ");
        message += this->ext;
    }
    return message;
}

#define EXCEPTION(NAME, PARENT)                                                \
    SimulatorException##NAME::SimulatorException##NAME(                        \
        QString reason, QString ext, QString file, int line)                   \
        : SimulatorException##PARENT(reason, ext, file, line) {                \
        name = #NAME;                                                          \
    }
SIMULATOR_EXCEPTIONS
#undef EXCEPTION
