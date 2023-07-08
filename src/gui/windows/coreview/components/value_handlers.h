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

class BoolValue {
public:
    BoolValue(svgscene::SimpleTextItem *element, const bool &data);
    void update();
    static const QString COMPONENT_NAME;

private:
    BORROWED svgscene::SimpleTextItem *const element;
    const bool &data;
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
    const machine::Address &data;
};

class RegValue {
public:
    RegValue(svgscene::SimpleTextItem *element, const machine::RegisterValue &data);
    void update();
    static const QString COMPONENT_NAME;

private:
    BORROWED svgscene::SimpleTextItem *const element;
    const machine::RegisterValue &data;
};

class RegIdValue {
public:
    RegIdValue(svgscene::SimpleTextItem *element, const machine::RegisterId &data);
    void update();
    static const QString COMPONENT_NAME;

private:
    BORROWED svgscene::SimpleTextItem *const element;
    const machine::RegisterId &data;
};

class DebugValue {
public:
    DebugValue(svgscene::SimpleTextItem *element, const unsigned int &data);
    void update();
    static const QString COMPONENT_NAME;

private:
    BORROWED svgscene::SimpleTextItem *const element;
    const unsigned &data;
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
    const machine::Instruction &instruction_data;
    const machine::Address &address_data;
};

template<typename SOURCE>
class Multiplexer {
public:
    Multiplexer(
        std::vector<BORROWED QGraphicsPathItem *> connections,
        const SOURCE &active_connection)
        : connections(std::move(connections))
        , active_connection(active_connection)
        , current_active_connection(0) {
        // Hide all but first
        for (size_t i = 1; i < this->connections.size(); ++i) {
            this->connections.at(i)->hide();
        }
    }
    void update() {
        if (current_active_connection != active_connection) {
            connections.at(static_cast<unsigned>(current_active_connection))->hide();
            connections.at(static_cast<unsigned>(active_connection))->show();
            current_active_connection = active_connection;
        }
    }

private:
    const std::vector<BORROWED QGraphicsPathItem *> connections;
    const SOURCE &active_connection;
    SOURCE current_active_connection;
};

#endif // QTRVSIM_VALUE_HANDLERS_H
