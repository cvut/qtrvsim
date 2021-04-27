#include "mainwindow.h"
#include "log.h"

#include <QApplication>

int main(int argc, char *argv[])
{
	NecroLog::setCLIOptions(argc, argv);

	QApplication a(argc, argv);
	MainWindow w;
	w.show();

	w.openFile(a.arguments().value(1));

	return a.exec();
}
