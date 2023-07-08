#include "messagesdock.h"

#include "assembler/messagetype.h"
#include "messagesmodel.h"
#include "messagesview.h"
#include "ui/hexlineedit.h"

#include <QHeaderView>
#include <QSettings>
#include <QVBoxLayout>
#include <QWidget>
#include <utility>

MessagesDock::MessagesDock(QWidget *parent, QSettings *settings)
    : Super(parent) {
    setObjectName("Messages");
    setWindowTitle("Messages");

    this->settings = settings;

    QWidget *content = new QWidget();

    QListView *messages_content = new MessagesView(nullptr, settings);
    MessagesModel *messages_model = new MessagesModel(this);
    messages_content->setModel(messages_model);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(messages_content);

    content->setLayout(layout);

    setWidget(content);

    connect(
        this, &MessagesDock::report_message, messages_model,
        &MessagesModel::insert_line);
    connect(
        this, &MessagesDock::pass_clear_messages, messages_model,
        &MessagesModel::clear_messages);
    connect(
        messages_content, &QAbstractItemView::activated, messages_model,
        &MessagesModel::activated);
    connect(
        messages_model, &MessagesModel::message_selected, this,
        &MessagesDock::message_selected);
}

void MessagesDock::insert_line(
    messagetype::Type type,
    QString file,
    int line,
    int column,
    QString text,
    QString hint) {
    report_message(
        type, std::move(file), line, column, std::move(text), std::move(hint));
}

void MessagesDock::clear_messages() {
    emit pass_clear_messages();
}
