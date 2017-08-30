#ifndef QTMIPSEXCEPTION_H
#define QTMIPSEXCEPTION_H

#include <exception>
#include <string>

#define QTMIPS_EXCEPTION(TYPE, REASON, EXT) (QtMipsException ## TYPE (std::string(REASON), std::string(EXT), std::string(__FILE__), __LINE__))
#define QTMIPS_ARGS_COMMON std::string reason, std::string ext, std::string file, int line

// Base exception for all machine ones
class QtMipsException : public std::exception {
public:
    QtMipsException(QTMIPS_ARGS_COMMON);
    const char *what() const throw();
    std::string msg(bool pos) const;
protected:
    std::string reason, ext, file;
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

// Instruction is jumping to unaligned address (ADDR%4!=0)
// This can be caused by bug or by user program as it can be jumping relative to register
// This shouldn't be happening with non-register jumps as those should be verified by compiler
class QtMipsExceptionUnalignedJump : public QtMipsExceptionRuntime {
public:
    QtMipsExceptionUnalignedJump(QTMIPS_ARGS_COMMON);
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
