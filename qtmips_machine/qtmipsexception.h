// SPDX-License-Identifier: GPL-2.0+
/*******************************************************************************
 * QtMips - MIPS 32-bit Architecture Subset Simulator
 *
 * Implemented to support following courses:
 *
 *   B35APO - Computer Architectures
 *   https://cw.fel.cvut.cz/wiki/courses/b35apo
 *
 *   B4M35PAP - Advanced Computer Architectures
 *   https://cw.fel.cvut.cz/wiki/courses/b4m35pap/start
 *
 * Copyright (c) 2017-2019 Karel Koci<cynerd@email.cz>
 * Copyright (c) 2019      Pavel Pisa <pisa@cmp.felk.cvut.cz>
 *
 * Faculty of Electrical Engineering (http://www.fel.cvut.cz)
 * Czech Technical University        (http://www.cvut.cz/)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 *
 ******************************************************************************/

#ifndef QTMIPSEXCEPTION_H
#define QTMIPSEXCEPTION_H

#include <exception>
#include <qstring.h>

namespace machine {

// Base exception for all machine ones
class QtMipsException : public std::exception {
public:
    QtMipsException(QString reason, QString ext, QString file, int line);
    const char *what() const noexcept override;
    QString msg(bool pos) const;

protected:
    QString name, reason, ext, file;
    int line;
};

/* This is list of all QtMips specific exceptions
 *
 * Input:
 *  Exception durring input loading
 * Runtime:
 *  Exceptions caused by machine invalid input or unsupported action
 * UnsupportedInstruction:
 *  Decoded instruction is not supported.
 *  This can be cause by really using some unimplemented instruction or because
 * of problems in instruction decode. UnsupportedAluOperation: Decoded ALU
 * operation is not supported This is basically same exception as
 * QtMipsExceptionUnsupportedInstruction but it is emmited from ALU when
 * executed and not before that. Overflow: Integer operation resulted to
 * overflow (or underflow as we are working with unsigned values) This is for
 * sure caused by program it self. UnalignedJump: Instruction is jumping to
 * unaligned address (ADDR % 4 != 0) This can be caused by bug or by user
 * program as it can be jumping relative to register This shouldn't be happening
 * with non-register jumps as those should be verified by compiler
 * UnknownMemoryControl:
 *  Used unknown MemoryAccess control value (write_ctl or read_ctl)
 *  This can be raised by invalid instruction but in such case we shoul raise
 * UnknownInstruction instead So this should signal just some QtMips bug.
 * OutOfMemoryAccess:
 *  Trying to access address outside of the memory
 *  As we are simulating whole 32bit memory address space then this is most
 * probably QtMips bug if raised not program. Sanity: This is sanity check
 * exception
 */
#define QTMIPS_EXCEPTIONS                                                      \
    EXCEPTION(Input, )                                                         \
    EXCEPTION(Runtime, )                                                       \
    EXCEPTION(UnsupportedInstruction, Runtime)                                 \
    EXCEPTION(UnsupportedAluOperation, Runtime)                                \
    EXCEPTION(Overflow, Runtime)                                               \
    EXCEPTION(UnalignedJump, Runtime)                                          \
    EXCEPTION(UnknownMemoryControl, Runtime)                                   \
    EXCEPTION(OutOfMemoryAccess, Runtime)                                      \
    EXCEPTION(Sanity, )                                                        \
    EXCEPTION(SyscallUnknown, Runtime)

#define EXCEPTION(NAME, PARENT)                                                \
    class QtMipsException##NAME : public QtMipsException##PARENT {             \
    public:                                                                    \
        QtMipsException##NAME(                                                 \
            QString reason,                                                    \
            QString ext,                                                       \
            QString file,                                                      \
            int line);                                                         \
    };
QTMIPS_EXCEPTIONS
#undef EXCEPTION

// This is helper macro for throwing QtMips exceptions
#define QTMIPS_EXCEPTION(TYPE, REASON, EXT)                                    \
    (machine::QtMipsException##TYPE(                                           \
        QString(REASON), QString(EXT), QString(__FILE__), __LINE__))

// Sanity comparison potentially throwing QtMipsExceptionSanity
#define SANITY_ASSERT(COND, MSG)                                               \
    do {                                                                       \
        if (!(COND))                                                           \
            throw QTMIPS_EXCEPTION(                                            \
                Sanity, "Sanity check failed (" #COND ")", MSG);               \
    } while (false)

} // namespace machine

#endif // QTMIPSEXCEPTION_H
