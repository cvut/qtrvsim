#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    // There constants are set in CMake.
    QApplication::setApplicationName(APP_NAME);
    QApplication::setOrganizationName(APP_ORGANIZATION);
    QApplication::setOrganizationDomain(APP_ORGANIZATION_DOMAIN);
    QApplication::setApplicationVersion(APP_VERSION);

    MainWindow w;
    w.start();

    return QApplication::exec();
}
