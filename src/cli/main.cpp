#include "common/logging.h"
#include "common/logging_format_colors.h"
#include "climain.h"

#include <QCoreApplication>

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName(APP_NAME);
    QCoreApplication::setApplicationVersion(APP_VERSION);
    set_default_log_pattern();

    CLIMain climain{app};

    QMetaObject::invokeMethod(&climain, "start", Qt::QueuedConnection);
    return QCoreApplication::exec();
}
