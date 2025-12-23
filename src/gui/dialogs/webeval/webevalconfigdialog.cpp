#include "webevalconfigdialog.h"

#include "mainwindow/mainwindow.h"
#include "windows/editor/editordock.h"
#include "windows/editor/srceditor.h"

#include <QDialogButtonBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QLabel>
#include <QMessageBox>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QPushButton>
#include <QVBoxLayout>

WebEvalConfigDialog::WebEvalConfigDialog(QSettings *settings, MainWindow *mainwindow, QWidget *parent)
    : QDialog(parent)
    , settings(settings)
    , mainwindow(mainwindow) {
    setWindowTitle("WebEval");
    setMinimumWidth(500);
    
    auto *layout = new QVBoxLayout(this);
    
    //config group
    auto *config_group = new QGroupBox("Configuration", this);
    auto *form_layout = new QFormLayout(config_group);
    
    url_edit = new QLineEdit(this);
    url_edit->setText(settings->value("webeval/url", "https://eval.comparch.edu.cvut.cz").toString());
    form_layout->addRow("URL:", url_edit);
    
    api_key_edit = new QLineEdit(this);
    api_key_edit->setEchoMode(QLineEdit::Password);
    api_key_edit->setText(settings->value("webeval/api_key", "").toString());
    form_layout->addRow("API Key:", api_key_edit);
    
    auto *save_btn = new QPushButton("Save Settings", this);
    connect(save_btn, &QPushButton::clicked, this, &WebEvalConfigDialog::save_settings);
    form_layout->addRow("", save_btn);
    
    layout->addWidget(config_group);
    

    //tasks list
    auto *tasks_group = new QGroupBox("Available Tasks", this);
    auto *tasks_layout = new QVBoxLayout(tasks_group);
    
    tasks_list = new QListWidget(this);
    tasks_list->setMinimumHeight(200);
    tasks_layout->addWidget(tasks_list);
    
    auto *refresh_btn = new QPushButton("Refresh Tasks", this);
    connect(refresh_btn, &QPushButton::clicked, this, &WebEvalConfigDialog::refresh_tasks);
    tasks_layout->addWidget(refresh_btn);
    
    layout->addWidget(tasks_group);
    
    
    //submit and load buttons
    auto *submit_layout = new QHBoxLayout();
    auto *submit_btn = new QPushButton("Submit Current File", this);
    connect(submit_btn, &QPushButton::clicked, this, &WebEvalConfigDialog::submit_current_file);
    submit_layout->addWidget(submit_btn);
    
    auto *load_template_btn = new QPushButton("Load Template", this);
    connect(load_template_btn, &QPushButton::clicked, this, &WebEvalConfigDialog::load_template);
    submit_layout->addWidget(load_template_btn);
    
    auto *load_latest_btn = new QPushButton("Load Latest", this);
    connect(load_latest_btn, &QPushButton::clicked, this, &WebEvalConfigDialog::load_latest);
    submit_layout->addWidget(load_latest_btn);
    
    auto *load_best_btn = new QPushButton("Load Best", this);
    connect(load_best_btn, &QPushButton::clicked, this, &WebEvalConfigDialog::load_best);
    submit_layout->addWidget(load_best_btn);
    
    submit_layout->addStretch();
    layout->addLayout(submit_layout);
    

    //close
    auto *button_box = new QDialogButtonBox(QDialogButtonBox::Close, this);
    connect(button_box, &QDialogButtonBox::rejected, this, &QDialog::accept);
    layout->addWidget(button_box);
    
    network_manager = new QNetworkAccessManager(this);
}

void WebEvalConfigDialog::save_settings() {
    settings->setValue("webeval/url", url_edit->text());
    settings->setValue("webeval/api_key", api_key_edit->text());
    QMessageBox::information(this, "Saved", "Settings saved successfully.");
}

void WebEvalConfigDialog::refresh_tasks() {
    QString url = url_edit->text().trimmed();
    QString api_key = api_key_edit->text().trimmed();
    
    if (url.isEmpty() || api_key.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please set URL and API Key first.");
        return;
    }
    
    QNetworkRequest request(QUrl(url + "/api/user/tasks"));
    request.setRawHeader("Authorization", ("Bearer " + api_key).toUtf8());
    
    QNetworkReply *reply = network_manager->get(request);
    connect(reply, &QNetworkReply::finished, this, &WebEvalConfigDialog::handle_tasks_reply);
}

void WebEvalConfigDialog::submit_current_file() {
    QString url = url_edit->text().trimmed();
    QString api_key = api_key_edit->text().trimmed();
    
    if (url.isEmpty() || api_key.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please set URL and API Key first.");
        return;
    }
    
    QListWidgetItem *selected = tasks_list->currentItem();
    if (!selected) {
        QMessageBox::warning(this, "Error", "Please select a task first.");
        return;
    }
    
    int task_id = selected->data(Qt::UserRole).toInt();
    
    //current focused editor content
    auto *editor = mainwindow->get_current_editor();
    if (!editor) {
        QMessageBox::warning(this, "Error", "No editor window is currently open.");
        return;
    }
    
    QString code = editor->toPlainText();
    
    //json data for the api call
    QJsonObject json;
    json["task_id"] = task_id;
    json["code"] = code;
    
    QNetworkRequest request(QUrl(url + "/api/user/submit"));
    request.setRawHeader("Authorization", ("Bearer " + api_key).toUtf8());
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    
    QNetworkReply *reply = network_manager->post(request, QJsonDocument(json).toJson());
    connect(reply, &QNetworkReply::finished, this, &WebEvalConfigDialog::handle_submit_reply);
}

void WebEvalConfigDialog::handle_tasks_reply() {
    auto *reply = qobject_cast<QNetworkReply *>(sender());
    if (!reply) return;
    
    tasks_list->clear();
    
    if (reply->error() != QNetworkReply::NoError) {
        QMessageBox::warning(this, "Error", "Failed to fetch tasks: " + reply->errorString());
        reply->deleteLater();
        return;
    }
    
    QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
    if (doc.isArray()) {
        QJsonArray tasks = doc.array();
        for (const QJsonValue &task : tasks) {
            QJsonObject obj = task.toObject();
            int id = obj["id"].toInt();
            QString name = obj["name"].toString();
            
            auto *item = new QListWidgetItem(QString("%1: %2").arg(id).arg(name));
            item->setData(Qt::UserRole, id);
            tasks_list->addItem(item);
        }
    } else if (doc.isObject()) {
        QJsonObject obj = doc.object();
        if (obj.contains("tasks")) {
            QJsonArray tasks = obj["tasks"].toArray();
            for (const QJsonValue &task : tasks) {
                QJsonObject task_obj = task.toObject();
                int id = task_obj["id"].toInt();
                QString name = task_obj["name"].toString();
                
                auto *item = new QListWidgetItem(QString("%1: %2").arg(id).arg(name));
                item->setData(Qt::UserRole, id);
                tasks_list->addItem(item);
            }
        }
    }
    
    reply->deleteLater();
}

void WebEvalConfigDialog::handle_submit_reply() {
    auto *reply = qobject_cast<QNetworkReply *>(sender());
    if (!reply) return;
    
    if (reply->error() != QNetworkReply::NoError) {
        QMessageBox::warning(this, "Error", "Submission failed: " + reply->errorString());
        reply->deleteLater();
        return;
    }
    
    QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
    QString message = "Submission successful!";
    
    if (doc.isObject()) {
        QJsonObject obj = doc.object();
        if (obj.contains("message")) {
            message = obj["message"].toString();
        }
    }
    
    QMessageBox::information(this, "Success", message);
    reply->deleteLater();
}

void WebEvalConfigDialog::load_template() {
    QString url = url_edit->text().trimmed();
    QString api_key = api_key_edit->text().trimmed();
    
    if (url.isEmpty() || api_key.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please set URL and API Key first.");
        return;
    }
    
    QListWidgetItem *selected = tasks_list->currentItem();
    if (!selected) {
        QMessageBox::warning(this, "Error", "Please select a task first.");
        return;
    }
    
    int task_id = selected->data(Qt::UserRole).toInt();
    
    QNetworkRequest request(QUrl(url + "/api/user/task/" + QString::number(task_id)));
    request.setRawHeader("Authorization", ("Bearer " + api_key).toUtf8());
    
    QNetworkReply *reply = network_manager->get(request);
    reply->setProperty("load_type", "template");
    connect(reply, &QNetworkReply::finished, this, &WebEvalConfigDialog::handle_load_reply);
}

void WebEvalConfigDialog::load_latest() {
    QString url = url_edit->text().trimmed();
    QString api_key = api_key_edit->text().trimmed();
    
    if (url.isEmpty() || api_key.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please set URL and API Key first.");
        return;
    }
    
    QListWidgetItem *selected = tasks_list->currentItem();
    if (!selected) {
        QMessageBox::warning(this, "Error", "Please select a task first.");
        return;
    }
    
    int task_id = selected->data(Qt::UserRole).toInt();
    
    QNetworkRequest request(QUrl(url + "/api/user/task/" + QString::number(task_id)));
    request.setRawHeader("Authorization", ("Bearer " + api_key).toUtf8());
    
    QNetworkReply *reply = network_manager->get(request);
    reply->setProperty("load_type", "latest");
    connect(reply, &QNetworkReply::finished, this, &WebEvalConfigDialog::handle_load_reply);
}

void WebEvalConfigDialog::load_best() {
    QString url = url_edit->text().trimmed();
    QString api_key = api_key_edit->text().trimmed();
    
    if (url.isEmpty() || api_key.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please set URL and API Key first.");
        return;
    }
    
    QListWidgetItem *selected = tasks_list->currentItem();
    if (!selected) {
        QMessageBox::warning(this, "Error", "Please select a task first.");
        return;
    }
    
    int task_id = selected->data(Qt::UserRole).toInt();
    
    QNetworkRequest request(QUrl(url + "/api/user/task/" + QString::number(task_id)));
    request.setRawHeader("Authorization", ("Bearer " + api_key).toUtf8());
    
    QNetworkReply *reply = network_manager->get(request);
    reply->setProperty("load_type", "best");
    connect(reply, &QNetworkReply::finished, this, &WebEvalConfigDialog::handle_load_reply);
}

void WebEvalConfigDialog::handle_load_reply() {
    auto *reply = qobject_cast<QNetworkReply *>(sender());
    if (!reply) return;
    
    if (reply->error() != QNetworkReply::NoError) {
        QMessageBox::warning(this, "Error", "Failed to load code: " + reply->errorString());
        reply->deleteLater();
        return;
    }
    
    QString load_type = reply->property("load_type").toString();
    QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
    QString code;
    
    if (doc.isObject()) {
        QJsonObject obj = doc.object();
        
        if (load_type == "template") {
            code = obj["template_code"].toString();
        } else if (load_type == "latest") {
            code = obj["submission_last"].toString();
        } else if (load_type == "best") {
            code = obj["submission_best"].toString();
        }
    }
    
    if (code.isEmpty()) {
        QMessageBox::warning(this, "Error", "No code found for " + load_type);
        reply->deleteLater();
        return;
    }
    
    //load code into focused editor
    auto *editor = mainwindow->get_current_editor();
    if (!editor) {
        QMessageBox::warning(this, "Error", "No editor window is currently open.");
        reply->deleteLater();
        return;
    }
    
    editor->setPlainText(code);
    QMessageBox::information(this, "Success", "Code loaded from " + load_type + " successfully.");
    reply->deleteLater();
}
