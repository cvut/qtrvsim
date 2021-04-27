#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "svghandler.h"

#include <QFileDialog>
#include <QGraphicsScene>
#include <QTimer>
#include <QXmlStreamReader>

using namespace svgscene;

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	m_scene = new QGraphicsScene(this);
	ui->graphicsView->setScene(m_scene);
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::openFile(const QString &fn)
{
	QString file_name = fn;
	if(file_name.isEmpty())
		file_name = QFileDialog::getOpenFileName(this, tr("Open Image"), "", tr("SVG Image Files (*.svg)"));
	if(file_name.isEmpty())
		return;
	QFile f(file_name);
	if(f.open(QFile::ReadOnly)) {
		m_scene->clear();
		QXmlStreamReader rd(&f);
		SvgHandler h(m_scene);
		h.load(&rd);
		QTimer::singleShot(0, ui->graphicsView, &SvgGraphicsView::zoomToFit);
	}
}

void MainWindow::on_action_Open_triggered()
{
	openFile();
}

void MainWindow::on_actionZoom_to_fit_triggered()
{
	ui->graphicsView->zoomToFit();
}
