#include "gotosymboldialog.h"
#include "ui_gotosymboldialog.h"

GoToSymbolDialog::GoToSymbolDialog(QWidget *parent, QStringList &symlist)
    : QDialog(parent), ui(new Ui::GoToSymbolDialog) {
  ui->setupUi(this);

  connect(ui->pushShowProg, &QAbstractButton::clicked, this,
          &GoToSymbolDialog::show_prog);
  connect(ui->pushShowMem, &QAbstractButton::clicked, this,
          &GoToSymbolDialog::show_mem);
  connect(ui->pushClose, &QAbstractButton::clicked, this, &QWidget::close);

  ui->listSymbols->addItems(symlist);
}

GoToSymbolDialog::~GoToSymbolDialog()
{
    delete ui;
}

void GoToSymbolDialog::show_prog() {
    std::uint32_t address = 0;
    emit obtain_value_for_name(address, ui->listSymbols->currentItem()->text());
    emit program_focus_addr(address);
}

void GoToSymbolDialog::show_mem() {
    std::uint32_t address = 0;
    emit obtain_value_for_name(address, ui->listSymbols->currentItem()->text());
    emit memory_focus_addr(address);
}
