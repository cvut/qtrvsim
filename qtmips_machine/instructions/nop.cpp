#include "nop.h"

QVector<QString> InstructionNop::to_strs() {
    QVector<QString> str;
    str << QString("nop");
    return str;
}
