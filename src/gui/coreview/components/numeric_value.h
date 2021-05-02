#ifndef QTRVSIM_NUMERIC_VALUE_H
#define QTRVSIM_NUMERIC_VALUE_H

#include <machine/memory/address.h>
#include <machine/register_value.h>
#include <svgscene/components/simpletextitem.h>
#include <svgscene/utils/memory_ownership.h>

class BoolValue {
public:
    BoolValue(svgscene::SimpleTextItem *element, const bool &data);
    void update();
    static constexpr const char *COMPONENT_NAME = "bool-value";

private:
    BORROWED svgscene::SimpleTextItem *const element;
    const bool &data;
};

class PCValue {
public:
    PCValue(svgscene::SimpleTextItem *element, const machine::Address &data);
    void update();
    static constexpr const char *COMPONENT_NAME = "pc-value";

private:
    BORROWED svgscene::SimpleTextItem *const element;
    const machine::Address &data;
};

class RegValue {
public:
    RegValue(svgscene::SimpleTextItem *element, const machine::RegisterValue &data);
    void update();
    static constexpr const char *COMPONENT_NAME = "reg-value";

private:
    BORROWED svgscene::SimpleTextItem *const element;
    const machine::RegisterValue &data;
};

class DebugValue {
public:
    DebugValue(svgscene::SimpleTextItem *element, const unsigned int &data);
    void update();
    static constexpr const char *COMPONENT_NAME = "reg-value";

private:
    BORROWED svgscene::SimpleTextItem *const element;
    const unsigned &data;
};

#endif // QTRVSIM_NUMERIC_VALUE_H
