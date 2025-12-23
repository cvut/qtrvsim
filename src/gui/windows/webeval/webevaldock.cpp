#include "webevaldock.h"

#include "mainwindow/mainwindow.h"
#include "windows/editor/editordock.h"
#include "windows/editor/srceditor.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QPushButton>
#include <QVBoxLayout>

WebEvalDock::WebEvalDock(QWidget *parent, QSettings *settings)
    : QDockWidget(parent)
    , settings(settings)
    , mainwindow(nullptr) {
    setObjectName("WebEval");
    setWindowTitle("WebEval");

    auto *content = new QWidget(this);
    auto *layout = new QVBoxLayout(content);

    tasks_list = new QListWidget(this);
    layout->addWidget(tasks_list);

    auto *refresh_btn = new QPushButton("Refresh Tasks", this);
    connect(refresh_btn, &QPushButton::clicked, this, &WebEvalDock::refresh_tasks);
    layout->addWidget(refresh_btn);

    auto *buttons_layout = new QHBoxLayout();
    
    auto *submit_btn = new QPushButton("Submit", this);
    connect(submit_btn, &QPushButton::clicked, this, &WebEvalDock::submit_current_file);
    buttons_layout->addWidget(submit_btn);

    auto *load_template_btn = new QPushButton("Load Template", this);
    connect(load_template_btn, &QPushButton::clicked, this, [this]() { load_code("template"); });
    buttons_layout->addWidget(load_template_btn);

    layout->addLayout(buttons_layout);

    auto *buttons_layout2 = new QHBoxLayout();
    
    auto *load_latest_btn = new QPushButton("Load Latest", this);
    connect(load_latest_btn, &QPushButton::clicked, this, [this]() { load_code("latest"); });
    buttons_layout2->addWidget(load_latest_btn);

    auto *load_best_btn = new QPushButton("Load Best", this);
    connect(load_best_btn, &QPushButton::clicked, this, [this]() { load_code("best"); });
    buttons_layout2->addWidget(load_best_btn);

    layout->addLayout(buttons_layout2);

    setWidget(content);
    network_manager = new QNetworkAccessManager(this);
}

void WebEvalDock::setup(MainWindow *mw) {
    mainwindow = mw;
}

void WebEvalDock::refresh_tasks() {
    QString url = settings->value("webeval/url", "https://eval.comparch.edu.cvut.cz").toString().trimmed();
    QString api_key = settings->value("webeval/api_key", "").toString().trimmed();

    if (url.isEmpty() || api_key.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please configure URL and API Key in Options > WebEval Configuration.");
        return;
    }

    QNetworkRequest request(QUrl(url + "/api/user/tasks"));
    request.setRawHeader("Authorization", ("Bearer " + api_key).toUtf8());

    QNetworkReply *reply = network_manager->get(request);
    connect(reply, &QNetworkReply::finished, this, &WebEvalDock::handle_tasks_reply);
}

void WebEvalDock::submit_current_file() {
    QString url = settings->value("webeval/url", "https://eval.comparch.edu.cvut.cz").toString().trimmed();
    QString api_key = settings->value("webeval/api_key", "").toString().trimmed();

    if (url.isEmpty() || api_key.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please configure URL and API Key in Options > WebEval Configuration.");
        return;
    }

    QListWidgetItem *selected = tasks_list->currentItem();
    if (!selected) {
        QMessageBox::warning(this, "Error", "Please select a task first.");
        return;
    }

    int task_id = selected->data(Qt::UserRole).toInt();

    if (!mainwindow) {
        QMessageBox::warning(this, "Error", "MainWindow not initialized.");
        return;
    }

    auto *editor = mainwindow->get_current_editor();
    if (!editor) {
        QMessageBox::warning(this, "Error", "No editor window is currently open.");
        return;
    }

    QString code = editor->toPlainText();

    QJsonObject json;
    json["task_id"] = task_id;
    json["code"] = code;

    QNetworkRequest request(QUrl(url + "/api/user/submit"));
    request.setRawHeader("Authorization", ("Bearer " + api_key).toUtf8());
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply *reply = network_manager->post(request, QJsonDocument(json).toJson());
    connect(reply, &QNetworkReply::finished, this, &WebEvalDock::handle_submit_reply);
}

void WebEvalDock::load_code(const QString &load_type) {
    QString url = settings->value("webeval/url", "https://eval.comparch.edu.cvut.cz").toString().trimmed();
    QString api_key = settings->value("webeval/api_key", "").toString().trimmed();

    if (url.isEmpty() || api_key.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please configure URL and API Key in Options > WebEval Configuration.");
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
    reply->setProperty("load_type", load_type);
    connect(reply, &QNetworkReply::finished, this, &WebEvalDock::handle_load_reply);
}

void WebEvalDock::handle_tasks_reply() {
    auto *reply = qobject_cast<QNetworkReply *>(sender());
    if (!reply) return;

    tasks_list->clear();

    if (reply->error() != QNetworkReply::NoError) {
        QMessageBox::warning(this, "Error", "Failed to fetch tasks: " + reply->errorString());
        reply->deleteLater();
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
    if (doc.isObject()) {
        QJsonObject obj = doc.object();
        if (obj.contains("tasks")) {
            QJsonArray tasks = obj["tasks"].toArray();
            for (const QJsonValue &task : tasks) {
                QJsonObject task_obj = task.toObject();
                int id = task_obj["id"].toInt();
                QString name = task_obj["name"].toString();

                //auto *item = new QListWidgetItem(QString("%1: %2").arg(id).arg(name));
                auto *item = new QListWidgetItem(QString("%1").arg(name));
                item->setData(Qt::UserRole, id);
                tasks_list->addItem(item);
            }
        }
    }

    reply->deleteLater();
}

void WebEvalDock::handle_submit_reply() {
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

void WebEvalDock::handle_load_reply() {
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

    if (!mainwindow) {
        QMessageBox::warning(this, "Error", "MainWindow not initialized.");
        reply->deleteLater();
        return;
    }

    auto *editor = mainwindow->get_current_editor();
    if (!editor) {
        QMessageBox::warning(this, "Error", "No editor window is currently open.");
        reply->deleteLater();
        return;
    }

    editor->setPlainText(code);
    //QMessageBox::information(this, "Success", "Code loaded from " + load_type + " successfully.");
    reply->deleteLater();
}
