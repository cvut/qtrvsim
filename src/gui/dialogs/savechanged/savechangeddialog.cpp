#include "savechangeddialog.h"

#include <QLabel>
#include <QListView>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QStandardItem>
#include <QTabWidget>
#include <QVBoxLayout>

SaveChnagedDialog::SaveChnagedDialog(QStringList &changedlist, QWidget *parent)
    : QDialog(parent) {
    setAttribute(Qt::WA_DeleteOnClose);
    setAttribute(Qt::WA_ShowModal);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setWindowTitle(tr("Save next modified files?"));

    model = new QStandardItemModel(this);
    bool unknown_inserted = false;

    for (const auto &fname : changedlist) {
        int row = model->rowCount();
        QStandardItem *item = new QStandardItem();
        item->setData(fname, Qt::UserRole);
        if (!fname.isEmpty()) {
            item->setText(fname);
        } else {
            if (!unknown_inserted) {
                item->setText("Unknown");
                unknown_inserted = true;
            }
        }
        item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        item->setCheckState(Qt::Checked);
        model->setItem(row, 0, item);
    }

    QVBoxLayout *all = new QVBoxLayout(this);

    QListView *listview = new QListView(this);
    listview->setModel(model);
    listview->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    all->addWidget(listview);

    QWidget *hbBtn = new QWidget();
    QHBoxLayout *hlBtn = new QHBoxLayout(hbBtn);

    QPushButton *cancelButton = new QPushButton(tr("&Cancel"), parent);
    QPushButton *ignoreButton = new QPushButton(tr("&Ignore"), parent);
    QPushButton *saveButton = new QPushButton(tr("&Save"), parent);
    saveButton->setFocus();
    connect(
        cancelButton, &QAbstractButton::clicked, this,
        &SaveChnagedDialog::cancel_clicked);
    connect(
        ignoreButton, &QAbstractButton::clicked, this,
        &SaveChnagedDialog::ignore_clicked);
    connect(
        saveButton, &QAbstractButton::clicked, this,
        &SaveChnagedDialog::save_clicked);
    hlBtn->addWidget(cancelButton);
    hlBtn->addStretch();
    hlBtn->addWidget(ignoreButton);
    hlBtn->addStretch();
    hlBtn->addWidget(saveButton);

    all->addWidget(hbBtn);

    setMinimumSize(400, 300);
}

SaveChnagedDialog::~SaveChnagedDialog() = default;

void SaveChnagedDialog::cancel_clicked() {
    QStringList list;
    emit user_decision(true, list);
    close();
}

void SaveChnagedDialog::ignore_clicked() {
    QStringList list;
    emit user_decision(false, list);
    close();
}

void SaveChnagedDialog::save_clicked() {
    QStringList list;
    for (int r = 0; r < model->rowCount(); ++r) {
        if (model->item(r)->checkState() == Qt::Checked) {
            list.append(model->item(r)->data(Qt::UserRole).toString());
        }
    }
    emit user_decision(false, list);
    close();
}
