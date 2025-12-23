#ifndef WEBEVALDOCK_H
#define WEBEVALDOCK_H

#include <QDockWidget>
#include <QListWidget>
#include <QNetworkAccessManager>
#include <QSettings>

class MainWindow;

class WebEvalDock : public QDockWidget {
    Q_OBJECT

public:
    WebEvalDock(QWidget *parent, QSettings *settings);
    void setup(MainWindow *mainwindow);

public slots:
    void refresh_tasks();
    void submit_current_file();
    void load_code(const QString &load_type);

private slots:
    void handle_tasks_reply();
    void handle_submit_reply();
    void handle_load_reply();

private:
    
    QSettings *settings;
    MainWindow *mainwindow;
    QListWidget *tasks_list;
    QNetworkAccessManager *network_manager;
};

#endif // WEBEVALDOCK_H
