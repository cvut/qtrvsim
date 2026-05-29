#include "webevalconfigdialog.h"

#include <QDialogButtonBox>
#include <QFormLayout>
#include <QVBoxLayout>

WebEvalConfigDialog::WebEvalConfigDialog(QSettings *settings, QWidget *parent)
    : QDialog(parent)
    , settings(settings) {
    setWindowTitle("WebEval Configuration");

    auto *layout = new QVBoxLayout(this);
    auto *form_layout = new QFormLayout();

    url_edit = new QLineEdit(this);
    url_edit->setText(settings->value("webeval/url", "https://eval.comparch.edu.cvut.cz").toString());
    form_layout->addRow("URL:", url_edit);

    api_key_edit = new QLineEdit(this);
    api_key_edit->setEchoMode(QLineEdit::Password);
    api_key_edit->setText(settings->value("webeval/api_key", "").toString());
    form_layout->addRow("API Key:", api_key_edit);

    layout->addLayout(form_layout);

    auto *button_box = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(button_box, &QDialogButtonBox::accepted, this, &WebEvalConfigDialog::save_and_close);
    connect(button_box, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(button_box);
}

void WebEvalConfigDialog::save_and_close() {
    settings->setValue("webeval/url", url_edit->text());
    settings->setValue("webeval/api_key", api_key_edit->text());
    accept();
}
