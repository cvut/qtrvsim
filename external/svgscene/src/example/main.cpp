#include "mainwindow.h"
#include "svgscene/utils/logging.h"

#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    w.openFile(QApplication::arguments().value(1));

    return QApplication::exec();
}
