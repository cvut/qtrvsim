#include "taskdescriptiondock.h"

#include <QVBoxLayout>

TaskDescriptionDock::TaskDescriptionDock(QWidget *parent)
    : QDockWidget(parent) {
    setObjectName("TaskDescription");
    setWindowTitle("Task Description");
    setMinimumSize(400, 300);
    
    setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);

    auto *content = new QWidget(this);
    auto *layout = new QVBoxLayout(content);
    layout->setContentsMargins(0, 0, 0, 0);

    text_browser = new QTextBrowser(this);
    text_browser->setOpenExternalLinks(true);
    layout->addWidget(text_browser);

    setWidget(content);
}

void TaskDescriptionDock::set_description(const QString &task_name, const QString &description) {
    setWindowTitle("Task: " + task_name);
    
    //use markdown if available
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    text_browser->setMarkdown(description);
#else
    //fallback to plain text or simple HTML
    text_browser->setPlainText(description);
#endif
}

void TaskDescriptionDock::clear_description() {
    setWindowTitle("Task Description");
    text_browser->clear();
}
