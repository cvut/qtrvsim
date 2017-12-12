#ifndef QTMIPSEXCEPTION_H
#define QTMIPSEXCEPTION_H

#include <exception>
#include <qstring.h>

#define QTMIPS_EXCEPTION(TYPE, REASON, EXT) (QtMipsException ## TYPE (QString(REASON), QString(EXT), QString(__FILE__), __LINE__))
#define QTMIPS_ARGS_COMMON QString reason, QString ext, QString file, int line

// Base exception for all machine ones
class QtMipsException : public std::exception {
public:
    QtMipsException(QTMIPS_ARGS_COMMON);
    const char *what() const throw();
    QString msg(bool pos) const;
protected:
    QString reason, ext, file;
    int line;
};

// Exception durring input loading
class QtMipsExceptionInput : public QtMipsException {
public:
    QtMipsExceptionInput(QTMIPS_ARGS_COMMON);
};

// Exceptions caused by machine invalid input or unsupported action
class QtMipsExceptionRuntime : public QtMipsException {
public:
    QtMipsExceptionRuntime(QTMIPS_ARGS_COMMON);
};

// Decoded instruction is not supported.
// This can be cause by really using some unimplemented instruction or because of problems in instruction decode.
class QtMipsExceptionUnsupportedInstruction : public QtMipsExceptionRuntime {
public:
    QtMipsExceptionUnsupportedInstruction(QTMIPS_ARGS_COMMON);
};

// Decoded ALU operation is not supported
// This is basically same exception as QtMipsExceptionUnsupportedInstruction but it is emmited from ALU when executed and not before that.
class QtMipsExceptionUnsupportedAluOperation : public QtMipsExceptionRuntime {
public:
    QtMipsExceptionUnsupportedAluOperation(QTMIPS_ARGS_COMMON);
};

// Integer operation resulted to overflow (or underflow as we are working with unsigned values)
// This is for sure caused by program it self.
class QtMipsExceptionOverflow : public QtMipsExceptionRuntime {
public:
    QtMipsExceptionOverflow(QTMIPS_ARGS_COMMON);
};

// Instruction is jumping to unaligned address (ADDR%4!=0)
// This can be caused by bug or by user program as it can be jumping relative to register
// This shouldn't be happening with non-register jumps as those should be verified by compiler
class QtMipsExceptionUnalignedJump : public QtMipsExceptionRuntime {
public:
    QtMipsExceptionUnalignedJump(QTMIPS_ARGS_COMMON);
};

// Used unknown MemoryAccess control value (write_ctl or read_ctl)
// This can be raised by invalid instruction but in such case we shoul raise UnknownInstruction instead
// So this should signal jsut some QtMips bug.
class QtMipsExceptionUnknownMemoryControl : public QtMipsExceptionRuntime {
public:
    QtMipsExceptionUnknownMemoryControl(QTMIPS_ARGS_COMMON);
};

// Trying to access address outside of the memory
// As we are simulating whole 32bit memory address space then this is most probably QtMips bug if raised not program.
class QtMipsExceptionOutOfMemoryAccess : public QtMipsExceptionRuntime {
public:
    QtMipsExceptionOutOfMemoryAccess(QTMIPS_ARGS_COMMON);
};

// This is sanity check exception
class QtMipsExceptionSanity : public QtMipsException {
public:
    QtMipsExceptionSanity(QTMIPS_ARGS_COMMON);
};

#define SANITY_ASSERT(COND, MSG) do { if (!(COND)) throw QTMIPS_EXCEPTION(Sanity, "Sanity check failed (" #COND ")", MSG); } while (false)

#endif // QTMIPSEXCEPTION_H
