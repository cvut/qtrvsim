#ifndef WEBEVALDOCK_H
#define WEBEVALDOCK_H

#include <QDockWidget>
#include <QListWidget>
#include <QNetworkAccessManager>
#include <QSettings>

class MainWindow;
class TaskDescriptionDock;

class WebEvalDock : public QDockWidget {
    Q_OBJECT

public:
    WebEvalDock(QWidget *parent, QSettings *settings);
    void setup(MainWindow *mainwindow, TaskDescriptionDock *description_dock);

public slots:
    void refresh_tasks();
    void submit_current_file();
    void load_code(const QString &load_type);
    void show_task_description();

private slots:
    void handle_tasks_reply();
    void handle_submit_reply();
    void handle_load_reply();
    void handle_task_detail_reply();

private:
    
    QSettings *settings;
    MainWindow *mainwindow;
    QListWidget *tasks_list;
    QNetworkAccessManager *network_manager;
    TaskDescriptionDock *description_dock;
};

#endif // WEBEVALDOCK_H
