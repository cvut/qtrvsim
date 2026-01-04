#ifndef TASKDESCRIPTIONDOCK_H
#define TASKDESCRIPTIONDOCK_H

#include <QDockWidget>
#include <QTextBrowser>

class TaskDescriptionDock : public QDockWidget {
    Q_OBJECT

public:
    explicit TaskDescriptionDock(QWidget *parent);
    void set_description(const QString &task_name, const QString &description);
    void clear_description();

private:
    QTextBrowser *text_browser;
};

#endif // TASKDESCRIPTIONDOCK_H
