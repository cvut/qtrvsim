#include <QApplication>
#include <QCommandLineParser>
#include "mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("qtmips_gui");
    app.setApplicationVersion("0.1");

    MainWindow w;
    w.start();

    return app.exec();
}
