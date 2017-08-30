#include "qtmipsexception.h"
#include <iostream>
#include <cstring>

QtMipsException::QtMipsException(QTMIPS_ARGS_COMMON) {
    this->reason = reason;
    this->ext = ext;
    this->file = file;
    this->line = line;
}

const char *QtMipsException::what() const throw() {
    std::string message = this->msg(true);
    char * cstr = new char [message.length()+1];
    std::strcpy (cstr, message.c_str());
    return cstr;
}

std::string QtMipsException::msg(bool pos) const {
    std::string message;
    if (pos)
        message += std::string("(") + std::string(this->file) + std::string(":") + std::to_string(this->line) + std::string(") ");
    message += this->reason;
    if (!this->ext.empty()) {
        message += std::string(": ");
        message += this->ext;
    }
    return message;
}


QtMipsExceptionInput::QtMipsExceptionInput(QTMIPS_ARGS_COMMON)
        : QtMipsException(reason, ext, file, line) {
    return;
}

QtMipsExceptionRuntime::QtMipsExceptionRuntime(QTMIPS_ARGS_COMMON)
        : QtMipsException(reason, ext, file, line) {
    return;
}

QtMipsExceptionUnsupportedInstruction::QtMipsExceptionUnsupportedInstruction(QTMIPS_ARGS_COMMON)
        : QtMipsExceptionRuntime(reason, ext, file, line) {
    return;
}

QtMipsExceptionUnalignedJump::QtMipsExceptionUnalignedJump(QTMIPS_ARGS_COMMON)
        : QtMipsExceptionRuntime(reason, ext, file, line) {
    return;
}

QtMipsExceptionOutOfMemoryAccess::QtMipsExceptionOutOfMemoryAccess(QTMIPS_ARGS_COMMON)
        : QtMipsExceptionRuntime(reason, ext, file, line) {
    return;
}

QtMipsExceptionSanity::QtMipsExceptionSanity(QTMIPS_ARGS_COMMON)
        : QtMipsException(reason, ext, file, line) {
    return;
}
