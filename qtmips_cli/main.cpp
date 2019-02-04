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

#include <QCoreApplication>
#include <QCommandLineParser>
#include <cctype>
#include <iostream>
#include "tracer.h"
#include "reporter.h"

using namespace machine;
using namespace std;

void create_parser(QCommandLineParser &p) {
    p.setApplicationDescription("QtMips CLI machine simulator");
    p.addHelpOption();
    p.addVersionOption();

    p.addPositionalArgument("FILE", "Input ELF executable file");

    p.addOptions({
        {"pipelined", "Configure CPU to use five stage pipeline."},
        {"no-delay-slot", "Disable jump delay slot."},
        {{"trace-fetch", "tr-fetch"}, "Trace fetched instruction (for both pipelined and not core)."},
        {{"trace-decode", "tr-decode"}, "Trace instruction in decode stage. (only for pipelined core)"},
        {{"trace-execute", "tr-execute"}, "Trace instruction in execute stage. (only for pipelined core)"},
        {{"trace-memory", "tr-memory"}, "Trace instruction in memory stage. (only for pipelined core)"},
        {{"trace-writeback", "tr-writeback"}, "Trace instruction in write back stage. (only for pipelined core)"},
        {{"trace-pc", "tr-pc"}, "Print program counter register changes."},
        {{"trace-gp", "tr-gp"}, "Print general purpose register changes. You can use * for all registers.", "REG"},
        {{"trace-lo", "tr-lo"}, "Print LO register changes."},
        {{"trace-hi", "tr-hi"}, "Print HI register changes."},
        {{"dump-registers", "d-regs"}, "Dump registers state at program exit."},
        {"expect-fail", "Expect that program causes CPU trap and fail if it doesn't."},
        {"fail-match", "Program should exit with exactly this CPU TRAP. Possible values are I(unsupported Instruction), A(Unsupported ALU operation), O(Overflow/underflow) and J(Unaligned Jump). You can freely combine them. Using this implies expect-fail option.", "TRAP"},
    });
}

void configure_machine(QCommandLineParser &p, MachineConfig &cc) {
    QStringList pa = p.positionalArguments();
    if (pa.size() != 1) {
        std::cerr << "Single ELF file has to be specified" << std::endl;
        exit(1);
    }
    cc.set_elf(pa[0]);

    cc.set_delay_slot(!p.isSet("no-delay-slot"));
    cc.set_pipelined(p.isSet("pipelined"));
}

void configure_tracer(QCommandLineParser &p, Tracer &tr) {
    if (p.isSet("trace-fetch"))
        tr.fetch();
    if (p.isSet("pipelined")) { // Following are added only if we have stages
        if (p.isSet("trace-decode"))
            tr.decode();
        if (p.isSet("trace-execute"))
            tr.execute();
        if (p.isSet("trace-memory"))
            tr.memory();
        if (p.isSet("trace-writeback"))
            tr.writeback();
    }

    if (p.isSet("trace-pc"))
        tr.reg_pc();

    QStringList gps = p.values("trace-gp");
    for (int i = 0; i < gps.size(); i++) {
        if (gps[i] == "*") {
            for (int y = 0; y < 32; y++)
                tr.reg_gp(y);
        } else {
            bool res;
            int num = gps[i].toInt(&res);
            if (res && num <= 32) {
                tr.reg_gp(num);
            } else {
                cout << "Unknown register number given for trace-gp: " << gps[i].toStdString() << endl;
                exit(1);
            }
        }
    }

    if (p.isSet("trace-lo"))
        tr.reg_lo();
    if (p.isSet("trace-hi"))
        tr.reg_hi();

    // TODO
}

void configure_reporter(QCommandLineParser &p, Reporter &r) {
    if (p.isSet("dump-registers"))
        r.regs();

    QStringList fail = p.values("fail-match");
    for (int i = 0; i < fail.size(); i++) {
        for (int y = 0; y < fail[i].length(); y++) {
            enum Reporter::FailReason reason;
            switch (tolower(fail[i].toStdString()[y])) {
            case 'i':
                reason = Reporter::FR_I;
                break;
            case 'a':
                reason = Reporter::FR_A;
                break;
            case 'o':
                reason = Reporter::FR_O;
                break;
            case 'j':
                reason = Reporter::FR_J;
                break;
            default:
                cout << "Unknown fail condition: " << fail[i].toStdString()[y] << endl;
                exit(1);
            }
            r.expect_fail(reason);
        }
    }
    if (p.isSet("expect-fail") && !p.isSet("fail-match"))
        r.expect_fail(Reporter::FailAny);

    // TODO
}

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    app.setApplicationName("qtmips_cli");
    app.setApplicationVersion("0.1");

    QCommandLineParser p;
    create_parser(p);
    p.process(app);

    MachineConfig cc;
    configure_machine(p, cc);
    QtMipsMachine machine(cc);

    Tracer tr(&machine);
    configure_tracer(p, tr);

    Reporter r(&app, &machine);
    configure_reporter(p, r);

    machine.play();
    return app.exec();
}
