#pragma once

#include "svgscene/components/simpletextitem.h"

#include <QMainWindow>
#include <QTimer>

class QGraphicsScene;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    void openFile(const QString &fn = QString());

private:
    Q_SLOT void on_action_Open_triggered();
    Q_SLOT void on_actionZoom_to_fit_triggered();
    Q_SLOT void inc();

private:
    Ui::MainWindow *ui;
    QGraphicsScene *m_scene;
    QTimer test;
    svgscene::SimpleTextItem *label;
    int counter = 0;
};