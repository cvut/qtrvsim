#ifndef WEBEVALCONFIGDIALOG_H
#define WEBEVALCONFIGDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QListWidget>
#include <QNetworkAccessManager>
#include <QSettings>

class MainWindow;

class WebEvalConfigDialog : public QDialog {
    Q_OBJECT

public:
    explicit WebEvalConfigDialog(QSettings *settings, MainWindow *mainwindow, QWidget *parent = nullptr);

private slots:
    void save_settings();
    void refresh_tasks();
    void submit_current_file();
    void load_template();
    void load_latest();
    void load_best();
    void handle_tasks_reply();
    void handle_submit_reply();
    void handle_load_reply();

private:
    QSettings *settings;
    MainWindow *mainwindow;
    QLineEdit *url_edit;
    QLineEdit *api_key_edit;
    QListWidget *tasks_list;
    QNetworkAccessManager *network_manager;
};

#endif // WEBEVALCONFIGDIALOG_H
