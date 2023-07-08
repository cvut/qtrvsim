#include "common/logging.h"
#include "common/logging_format_colors.h"
#include "mainwindow/mainwindow.h"

#include <QApplication>

LOG_CATEGORY("gui.main");

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    set_default_log_pattern();

    // There constants are set in CMake.
    QApplication::setApplicationName(APP_NAME);
    QApplication::setOrganizationName(APP_ORGANIZATION);
    QApplication::setOrganizationDomain(APP_ORGANIZATION_DOMAIN);
    QApplication::setApplicationVersion(APP_VERSION);

    LOG("Started %s version %s.", APP_NAME, APP_VERSION);
    LOG("Developed at %s (%s).", APP_ORGANIZATION, APP_ORGANIZATION_DOMAIN);

    /*
     * If environment variable specified in define ENV_CONFIG_FILE_NAME is
     * present, the app should behave in a portable manner and use ini file on the
     * specified location. Otherwise, Qt defaults are used.
     */
    auto env = QProcessEnvironment::systemEnvironment();
    QSettings *settings;
    if (env.contains(ENV_CONFIG_FILE_NAME)) {
        // Behave as a portable app.
        settings = new QSettings(env.value(ENV_CONFIG_FILE_NAME), QSettings::IniFormat);
    } else {
        // Qt defaults
        settings = new QSettings(APP_ORGANIZATION, APP_NAME);
    }

    MainWindow w(settings); // Moving settings ownership.
    w.start();

    return QApplication::exec();
}
