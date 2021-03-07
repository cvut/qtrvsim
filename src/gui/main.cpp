#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QApplication::setApplicationName("gui");
    QApplication::setApplicationVersion("0.7.5");

    MainWindow w;
    w.start();

    return QApplication::exec();
}
