/**
 * Components defined in here update the GUI placeholders with up to date
 * values that is read from provided source.
 *
 * Components accept different types and produce different formatting.
 *
 * @file
 */
#ifndef QTRVSIM_VALUE_HANDLERS_H
#define QTRVSIM_VALUE_HANDLERS_H

#include <QList>
#include <instruction.h>
#include <machine/memory/address.h>
#include <machine/register_value.h>
#include <machine/registers.h>
#include <svgscene/components/simpletextitem.h>
#include <svgscene/utils/memory_ownership.h>
#include <unordered_map>
#include <utility>
#include <vector>

/**
 * Proxy for a value that can be fetched from a reference and compared to previous value.
 */
template<typename T>
class ValueProxy {
public:
    ValueProxy(const T &data) : data(data), last_value(data) {}

    /**
     * Fetch new value and return true if it changed from last time this function was called.
     */
    bool fetch_and_compare() {
        if (data != last_value) {
            last_value = data;
            return true;
        }
        return false;
    }

    /**
     * Get the last fetched value.
     */
    const T &get() const { return last_value; }

    /**
     * Get remote value directly.
     */
    const T &get_direct() const { return data; }

private:
    const T &data;
    T last_value;
};

class BoolValue {
public:
    BoolValue(svgscene::SimpleTextItem *element, const bool &data);
    void update();
    static const QString COMPONENT_NAME;

private:
    BORROWED svgscene::SimpleTextItem *const element;
    ValueProxy<bool> data;

    static const QString TRUE_STRING;
    static const QString FALSE_STRING;
};

class PCValue : public QObject {
    Q_OBJECT

public slots:
    void clicked();
signals:
    void jump_to_pc(machine::Address pc_value);

public:
    PCValue(svgscene::SimpleTextItem *element, const machine::Address &data);
    PCValue(const PCValue &);
    void update();
    static const QString COMPONENT_NAME;

private:
    BORROWED svgscene::SimpleTextItem *const element;
    ValueProxy<machine::Address> data;
    QString buffer { "0x00000000" };
};

class RegValue {
public:
    RegValue(svgscene::SimpleTextItem *element, const machine::RegisterValue &data);
    void update();
    static const QString COMPONENT_NAME;

private:
    BORROWED svgscene::SimpleTextItem *const element;
    ValueProxy<machine::RegisterValue> data;
    QString buffer { "00000000" };
};

class RegIdValue {
public:
    RegIdValue(svgscene::SimpleTextItem *element, const machine::RegisterId &data);
    void update();
    static const QString COMPONENT_NAME;
    QString buffer { "00" };

private:
    BORROWED svgscene::SimpleTextItem *const element;
    ValueProxy<machine::RegisterId> data;
};

class DebugValue {
public:
    DebugValue(svgscene::SimpleTextItem *element, const unsigned int &data);
    void update();
    static const QString COMPONENT_NAME;

private:
    BORROWED svgscene::SimpleTextItem *const element;
    ValueProxy<unsigned> data;
    QString buffer { "0" };
};

class MultiTextValue {
    using Source = const std::unordered_map<unsigned, QString> &;
    using Data = std::pair<const unsigned int &, Source>;

public:
    MultiTextValue(svgscene::SimpleTextItem *element, Data data);
    void update();
    static const QString COMPONENT_NAME;

private:
    BORROWED svgscene::SimpleTextItem *const element;
    const unsigned &current_text_index;
    unsigned last_text_index;
    Source &text_table;
    QBrush originalBrush;
};

class InstructionValue {
    using Data = std::pair<const machine::Instruction &, const machine::Address &>;

public:
    InstructionValue(svgscene::SimpleTextItem *element, Data data);
    void update();
    static const QString COMPONENT_NAME;

private:
    BORROWED svgscene::SimpleTextItem *const element;
    ValueProxy<machine::Instruction> instruction_data;
    ValueProxy<machine::Address> address_data;
};

template<typename SOURCE>
class Multiplexer {
public:
    Multiplexer(
        std::vector<BORROWED QGraphicsPathItem *> connections,
        const SOURCE &active_connection)
        : connections(std::move(connections))
        , active_connection(active_connection) {
        // Hide all but first
        for (size_t i = 1; i < this->connections.size(); ++i) {
            this->connections.at(i)->hide();
        }
    }
    void update() {
        auto old_connection = active_connection.get();
        if (active_connection.fetch_and_compare()) {
            connections.at(static_cast<unsigned>(old_connection))->hide();
            connections.at(static_cast<unsigned>(active_connection.get()))->show();
        }
    }

private:
    const std::vector<BORROWED QGraphicsPathItem *> connections;
    ValueProxy<SOURCE> active_connection;
};

#endif // QTRVSIM_VALUE_HANDLERS_H
