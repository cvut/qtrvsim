#include "terminaldock.h"

#include "machine/memory/backend/serialport.h"

#include <QString>
#include <QTextBlock>
#include <QTextCursor>

TerminalDock::TerminalDock(QWidget *parent, QSettings *settings) : QDockWidget(parent) {
    (void)settings;
    top_widget = new QWidget(this);
    setWidget(top_widget);
    layout_box = new QVBoxLayout(top_widget);

    terminal_text = new QTextEdit(top_widget);
    terminal_text->setMinimumSize(30, 30);
    layout_box->addWidget(terminal_text);
    append_cursor.reset(new QTextCursor(terminal_text->document()));
    layout_bottom_box = new QHBoxLayout();
    layout_bottom_box->addWidget(new QLabel("Input:"));
    input_edit = new QLineEdit();
    layout_bottom_box->addWidget(input_edit);
    layout_box->addLayout(layout_bottom_box);
    // insert newline on enter (it will be displayed as space)
    connect(input_edit, &QLineEdit::returnPressed, [this]() {
        input_edit->setText(input_edit->text() + '\n');
    });

    setObjectName("Terminal");
    setWindowTitle("Terminal");
}

void TerminalDock::setup(machine::SerialPort *ser_port) {
    if (ser_port == nullptr) { return; }
    connect(
        ser_port, &machine::SerialPort::tx_byte, this,
        QOverload<unsigned int>::of(&TerminalDock::tx_byte));
    connect(ser_port, &machine::SerialPort::rx_byte_pool, this, &TerminalDock::rx_byte_pool);
    connect(input_edit, &QLineEdit::textChanged, ser_port, &machine::SerialPort::rx_queue_check);
}

void TerminalDock::tx_byte(unsigned int data) {
    bool at_end = terminal_text->textCursor().atEnd();
    if (data == '\n') {
        append_cursor->insertBlock();
    } else {
        append_cursor->insertText(QString(QChar(data)));
    }
    if (at_end) {
        QTextCursor cursor = QTextCursor(terminal_text->document());
        cursor.movePosition(QTextCursor::End);
        terminal_text->setTextCursor(cursor);
    }
}

void TerminalDock::tx_byte(int fd, unsigned int data) {
    (void)fd;
    tx_byte(data);
}

void TerminalDock::rx_byte_pool(int fd, unsigned int &data, bool &available) {
    (void)fd;
    QString str = input_edit->text();
    available = false;
    if (str.count() > 0) {
        data = str[0].toLatin1();
        input_edit->setText(str.remove(0, 1));
        available = true;
    }
}
