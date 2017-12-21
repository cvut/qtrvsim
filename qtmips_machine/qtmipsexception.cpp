#include "qtmipsexception.h"
#include <iostream>
#include <cstring>

using namespace machine;

QtMipsException::QtMipsException(QTMIPS_ARGS_COMMON) {
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
    if (pos)
        message += QString("(") + QString(this->file) + QString(":") + QString::number(this->line) + QString(") ");
    message += this->reason;
    if (!this->ext.isEmpty()) {
        message += QString(": ");
        message += this->ext;
    }
    return message;
}


QtMipsExceptionInput::QtMipsExceptionInput(QTMIPS_ARGS_COMMON)
        : QtMipsException(reason, ext, file, line) { }

QtMipsExceptionRuntime::QtMipsExceptionRuntime(QTMIPS_ARGS_COMMON)
        : QtMipsException(reason, ext, file, line) { }

QtMipsExceptionUnsupportedInstruction::QtMipsExceptionUnsupportedInstruction(QTMIPS_ARGS_COMMON)
        : QtMipsExceptionRuntime(reason, ext, file, line) { }

QtMipsExceptionUnsupportedAluOperation::QtMipsExceptionUnsupportedAluOperation(QTMIPS_ARGS_COMMON)
        : QtMipsExceptionRuntime(reason, ext, file, line) { }

QtMipsExceptionOverflow::QtMipsExceptionOverflow(QTMIPS_ARGS_COMMON)
        : QtMipsExceptionRuntime(reason, ext, file, line) { }

QtMipsExceptionUnalignedJump::QtMipsExceptionUnalignedJump(QTMIPS_ARGS_COMMON)
        : QtMipsExceptionRuntime(reason, ext, file, line) { }

QtMipsExceptionUnknownMemoryControl::QtMipsExceptionUnknownMemoryControl(QTMIPS_ARGS_COMMON)
        : QtMipsExceptionRuntime(reason, ext, file, line) { }

QtMipsExceptionOutOfMemoryAccess::QtMipsExceptionOutOfMemoryAccess(QTMIPS_ARGS_COMMON)
        : QtMipsExceptionRuntime(reason, ext, file, line) { }

QtMipsExceptionSanity::QtMipsExceptionSanity(QTMIPS_ARGS_COMMON)
        : QtMipsException(reason, ext, file, line) { }
