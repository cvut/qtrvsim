#include "simulator_exception.h"

#include <cstring>
#include <iostream>
#include <utility>

using namespace machine;

SimulatorException::SimulatorException(
    QString reason,
    QString ext,
    QString file,
    int line,
    ExceptionCause cause,
    Address fault_address) {
    this->name = "Exception";
    this->reason = std::move(reason);
    this->ext = std::move(ext);
    this->file = std::move(file);
    this->line = line;
    this->cached_what = nullptr;
    this->cause_ = cause;
    this->fault_address_ = fault_address;
}

SimulatorException::~SimulatorException() {
    if (this->cached_what) { delete[] this->cached_what; }
}

const char *SimulatorException::what() const noexcept {
    if (this->cached_what) { return this->cached_what; }

    std::string message = this->msg(true).toStdString();
    this->cached_what = new char[message.length() + 1];
    std::strcpy(this->cached_what, message.c_str());

    return this->cached_what;
}

QString SimulatorException::msg(bool pos) const {
    QString message;
    message += name;
    if (pos) {
        message += QString(" (") + QString(this->file) + QString(":") + QString::number(this->line)
                   + QString(")");
    }
    message += ": " + this->reason;
    if (!this->ext.isEmpty()) {
        message += QString(": ");
        message += this->ext;
    }
    return message;
}

#define EXCEPTION(NAME, PARENT)                                                                    \
    SimulatorException##NAME::SimulatorException##NAME(                                            \
        QString reason, QString ext, QString file, int line, ExceptionCause cause,                 \
        Address fault_address)                                                                     \
        : SimulatorException##PARENT(reason, ext, file, line, cause, fault_address) {              \
        this->name = #NAME;                                                                        \
    }
SIMULATOR_EXCEPTIONS
#undef EXCEPTION
