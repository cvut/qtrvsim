#ifndef TERMINALDOCK_H
#define TERMINALDOCK_H

#include "machine/machine.h"

#include <QDockWidget>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTextCursor>
#include <QTextEdit>

class TerminalDock : public QDockWidget {
    Q_OBJECT
public:
    TerminalDock(QWidget *parent, QSettings *settings);

    void setup(machine::SerialPort *ser_port);

public slots:
    void tx_byte(unsigned int data);
    void tx_byte(int fd, unsigned int data);
    void rx_byte_pool(int fd, unsigned int &data, bool &available);

private:
    QVBoxLayout *layout_box;
    QHBoxLayout *layout_bottom_box;
    QWidget *top_widget, *top_form {};
    QFormLayout *layout_top_form {};
    QTextEdit *terminal_text;
    Box<QTextCursor> append_cursor;
    QLineEdit *input_edit;
};

#endif // TERMINALDOCK_H
